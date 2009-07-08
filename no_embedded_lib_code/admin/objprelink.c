/* objprelink -- prelinks c++ files for producing a more efficient dynlink.
   Copyright 2001, Leon Bottou

   This program is free software; you can redistribute it and/or modify ait
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option)
   any later version.  

   Compile with: 
      gcc -O2 -o objprelink objprelink.c /usr/lib/libbfd.a /usr/lib/libiberty.a

   This is i386 only.
   There is very experimental PPC support (totally unproven).
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "bfd.h"

static int verbose = 0;
static int replace = 1;
static const char *target = 0;
static const char *program_name = "objprelink";
static const char *enomem = "out of memory";
static const char *eassert = "assertion failed";

static void
fatal(const char *fname, int lineno, const char *msg)
{
  if (program_name)
    fprintf(stderr,"%s ", program_name);
  if (fname)
    fprintf(stderr,"(%s:%d) ", fname, lineno);
  if (program_name || fname)
    fprintf(stderr," : ");
  if (bfd_get_error() != bfd_error_no_error)
    bfd_perror(msg);
  else if (errno)
    perror(msg);
  else 
    fprintf(stderr,"%s\n", msg);
  exit(5);
}

#define FATAL(s) \
  do{fatal(__FILE__,__LINE__,s);}while(0)
#define ASSERT(x,s) \
  do{if(!(x))fatal(__FILE__,__LINE__,s);}while(0)
#define CLEARERR() \
  do{errno=0;bfd_set_error(bfd_error_no_error);}while(0)

static const char *
M(const char *format, ...)
{
  static char buffer[256*256];
  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);
  return buffer;
}

static void
usage(FILE *stream, int status)
{
  fprintf(stream,
          "Usage: %s [-v] [-n] <objectfiles>\n"
          "-- \"prelink\" c++ virtual tables\n"
            "\n"
          "This program massages object files in order to accelerate the loading\n"
          "of shared object files (.so files) created by linking these object files.\n"
          "This is intel specific at the moment\n"
          "\n"
          "The program scans the specified object file for linkonce sections\n"
          "containing virtual tables with 32 bit absolute relocations (R_386_32)\n"
          "to undefined procedures.  For each such procedure, it creates a linkonce\n"
          "text section containing a relay jump.  All absolute relocations to this\n"
          "procedure are then redirected to the single relay jump.\n"
          "\n"
          "Program objprelink should be called on all object files before linking.\n"
          "\n"
          "Option -v selects verbose mode\n"
          "Option -n does not change the object files but generates a new object\n"
          "whose name is composed by appending \".new\" to the original filename.\n",
          program_name);
  exit(status);
}


/* ---------------- MACHINE DEPENDENT PART ---- */

static char *stub;
static int stub_size;
static int stub_alignment; 
static void (*stub_install)(bfd*, asection*, asymbol**);

static reloc_howto_type *howto_32;


/* I386 */

static char stub_i386[] = {
  0xb8, 0, 0, 0, 0,               /* mov $0,%eax */
  0xff, 0xe0                      /* jmp *(%eax) */
};

static void 
stub_install_i386(bfd *obfd, asection *p, asymbol **jumpto)
{
  int status;
  int relcount = 1;
  arelent **reloc = (arelent**)bfd_alloc(obfd, sizeof(arelent*)*relcount);
  arelent *relent = (arelent*)bfd_alloc(obfd, sizeof(arelent)*relcount);
  ASSERT(reloc, enomem);
  ASSERT(relent, enomem);
  reloc[0] = &relent[0];
  reloc[0]->sym_ptr_ptr = jumpto;
  reloc[0]->address = 1;
  reloc[0]->addend = 0;
  reloc[0]->howto = howto_32;
  bfd_set_reloc(obfd, p, reloc, relcount);
  status = bfd_set_section_contents(obfd, p, stub, (file_ptr) 0, stub_size);
  ASSERT(status, eassert);
}

/* PPC */

static char stub_ppc[] = {
  0x48, 0, 0, 0                    
};

static reloc_howto_type *howto_ppc_pltrel24;
static reloc_howto_type *howto_ppc_rel24;

static void
stub_install_ppc(bfd *obfd, asection *p, asymbol **jumpto)
{
  int status;
  int relcount = 1;
  arelent **reloc = (arelent**)bfd_alloc(obfd, sizeof(arelent*)*relcount);
  arelent *relent = (arelent*)bfd_alloc(obfd, sizeof(arelent)*relcount);
  ASSERT(reloc, enomem);
  ASSERT(relent, enomem);
  reloc[0] = &relent[0];
  reloc[0]->sym_ptr_ptr = jumpto;
  reloc[0]->address = 0;
  reloc[0]->addend = 0;
  reloc[0]->howto = howto_ppc_pltrel24;  /* Second choice: howto_ppc_rel24 */
  bfd_set_reloc(obfd, p, reloc, relcount);
  status = bfd_set_section_contents(obfd, p, stub, (file_ptr) 0, stub_size);
  ASSERT(status, eassert);
}

/* ALPHA */

/* ALL */

static void
prepare_stub_data(bfd *obfd, asymbol **syms, int symcount)
{
  switch (bfd_get_arch(obfd))
    {
    case bfd_arch_i386:
      stub = stub_i386;
      stub_size = sizeof(stub_i386);
      stub_install = stub_install_i386;
      stub_alignment = 1;
      break;
      
    case bfd_arch_powerpc:
      fprintf(stderr,"Warning: PPC support is experimental\n");
      howto_ppc_pltrel24 = bfd_reloc_type_lookup(obfd, BFD_RELOC_24_PLT_PCREL);
      howto_ppc_rel24 = bfd_reloc_type_lookup(obfd, BFD_RELOC_PPC_B26);
      ASSERT(howto_ppc_pltrel24, eassert);
      ASSERT(howto_ppc_rel24, eassert);
      stub = stub_ppc;
      stub_size = sizeof(stub_ppc);
      stub_install = stub_install_ppc;
      stub_alignment = 2;
      break;

    default:
      FATAL("This cpu architecture is not supported (yet)");
      break;
    }
  howto_32 =  bfd_reloc_type_lookup(obfd, BFD_RELOC_32);
  ASSERT(howto_32, eassert);
}


/* ---------- END OF MACHINE DEPENDENT PART ---- */



/* Symbol hash table */

typedef struct symbol_entry_s {
  struct bfd_hash_entry root;
  struct symbol_entry_s *next;
  asection *relay;
  asymbol  *sym;
} symbol_entry;

static struct bfd_hash_entry *
init_symbol_entry(struct bfd_hash_entry *entry, 
                  struct bfd_hash_table *table,
                  const char *string)
{
  symbol_entry *ret = (symbol_entry*)entry;
  if (! ret)
    ret = (symbol_entry*)bfd_hash_allocate(table, sizeof(symbol_entry));
  ASSERT(ret,eassert);
  memset(ret, 0, sizeof(symbol_entry));
  return bfd_hash_newfunc((struct bfd_hash_entry*)ret, table, string);
}

static symbol_entry *
lookup_symbol(struct bfd_hash_table *table, const char *id, boolean create)
{
  return (symbol_entry*) bfd_hash_lookup(table, id, create, true);
}

/* Cached relocations stored 
   in section userdata field */

struct section_userdata {
  int relcount;
  arelent **reloc;
};

/* Check if section is a virtual table */

static const char *
is_vt_section(bfd *abfd, asection *p)
{
  const char *name = bfd_section_name(abfd, p);
  static const char prefix[] = ".gnu.linkonce.d.__vt_";
  static flagword w = (SEC_ALLOC|SEC_LOAD|SEC_RELOC|SEC_DATA|SEC_LINK_ONCE);
  flagword f = bfd_get_section_flags (abfd, p);
  if (f & w == w)
    if (!strncmp(name, prefix, sizeof(prefix)-1))
      return name + sizeof(prefix)-6;
  return false;
}

/* Process one object file.
   Returns true if a new file has been successfully created. */

static boolean
process(char *iname, char *oname)
{

  bfd *ibfd = 0;
  bfd *obfd = 0;
  int isymcount = 0;
  asymbol **isyms = 0;
  struct bfd_hash_table vtsymtab;
  symbol_entry *vtsyms = 0;
  int vtcount = 0;

  int relcount;
  arelent **reloc;
  asection *p;
  symbol_entry *hsym;
  int storage_needed;
  int status;
  int i;

  if (verbose)
    printf("processing file %s\n", iname);

  /* --- open bfds */
  ibfd = bfd_openr (iname, target);
  ASSERT(ibfd, M("cannot read file \"%s\"", iname));
  if (! bfd_check_format(ibfd, bfd_object))
    FATAL(M("file \"%s\" is not a regular object file", iname));
  CLEARERR();

  /* --- get input symbols */
  storage_needed = bfd_get_symtab_upper_bound(ibfd);
  ASSERT(storage_needed>=0, eassert);
  isyms = (asymbol**) bfd_alloc(ibfd, storage_needed);
  ASSERT(isyms, enomem);
  isymcount = bfd_canonicalize_symtab(ibfd, isyms);
  ASSERT(isymcount>=0, eassert);
  CLEARERR();

  /* --- prepare variables required for stub generation */
  prepare_stub_data(ibfd, isyms, isymcount);

  /* --- cache all relocation entries */
  for (p=ibfd->sections; p; p=p->next)
    {
      const char *name = bfd_section_name(ibfd, p);
      if (! strncmp(name, ".gnu.linkonce.t.stub.", 21))
        {
          if (verbose)
            printf(" file has already been processed\n");
          bfd_close(ibfd);
          return false;
        }
      p->userdata = 0;
      storage_needed = bfd_get_reloc_upper_bound(ibfd, p);
      if (storage_needed > 0)
        {
          struct section_userdata *u = (void*)bfd_alloc(ibfd, sizeof(*u));
          ASSERT(u, enomem);
          memset(u, 0, sizeof(*u));
          reloc = (arelent**)bfd_alloc(ibfd, storage_needed);
          ASSERT(reloc, enomem);
          relcount = bfd_canonicalize_reloc(ibfd, p, reloc, isyms);
          ASSERT(relcount>=0, M("Problem with relocations for section %s in file %s", 
                                bfd_section_name(ibfd,p), bfd_get_filename(ibfd)));
          u->relcount = relcount;
          u->reloc = reloc;
          p->userdata = u;
        }
    }

  /* --- construct hash table of vt undefined symbols */
  memset(&vtsymtab, 0, sizeof(vtsymtab));
  status = bfd_hash_table_init(&vtsymtab, init_symbol_entry);
  ASSERT(status, eassert);
  for (p=ibfd->sections; p; p=p->next)
    {
      const char *vtname = is_vt_section(ibfd, p);
      if (vtname)
        {
          if (verbose)
            printf(" scanning section \"%s\"\n",  vtname);
          reloc = (p->userdata) ? ((struct section_userdata*)p->userdata)->reloc : 0;
          relcount = (p->userdata) ? ((struct section_userdata*)p->userdata)->relcount : 0;
          for (i=0; i<relcount; i++)
            {
              arelent *rel = reloc[i];
              asymbol *sym = (rel->sym_ptr_ptr ? *rel->sym_ptr_ptr : 0);
              if (sym &&
                  rel->howto->type == howto_32->type && 
                  !lookup_symbol(&vtsymtab, sym->name, false))
                {
                  hsym = lookup_symbol(&vtsymtab, sym->name, true);
                  ASSERT(hsym, eassert);
                  hsym->next = vtsyms;
                  hsym->sym = sym;
                  vtsyms = hsym;
                  vtcount += 1;
                }
            }
        }
    }
  if (verbose)
    printf(" found %d symbols in need for a stub\n", vtcount);
  if (! vtcount) 
    {
      bfd_hash_table_free(&vtsymtab);
      bfd_close(ibfd);
      return false;
    }
      
  /* --- create output bfd */
  obfd = bfd_openw (oname, bfd_get_target (ibfd));
  ASSERT(obfd, M("cannot create file \"%s\"", iname));
  bfd_set_format (obfd, bfd_get_format (ibfd));
  bfd_set_start_address (obfd, bfd_get_start_address(ibfd));
  bfd_set_file_flags (obfd, (bfd_get_file_flags (ibfd) & bfd_applicable_file_flags (obfd)));
  bfd_set_arch_mach (obfd, bfd_get_arch (ibfd), bfd_get_mach (ibfd));

  /* --- create copied sections */
  for (p=ibfd->sections; p; p=p->next)
    {
      const char *err = M("cannot replicate section %s from file %s", 
                          bfd_section_name (ibfd, p), iname);
      asection *isection = p;
      asection *osection = bfd_make_section_anyway (obfd, bfd_section_name (ibfd, isection));
      ASSERT(osection, err);
      status = bfd_set_section_size (obfd, osection, bfd_section_size (ibfd, isection));
      ASSERT(status, err);
      status = bfd_set_section_vma (obfd, osection, bfd_section_vma (ibfd, isection));
      ASSERT(status, err);
      osection->lma = isection->lma;
      status = bfd_set_section_alignment(obfd, osection, bfd_section_alignment (ibfd, isection));
      ASSERT(status, err);
      status = bfd_set_section_flags(obfd, osection, bfd_get_section_flags (ibfd, isection));
      ASSERT(status, err);
      isection->output_section = osection;
      isection->output_offset = 0;
      status = bfd_copy_private_section_data (ibfd, isection, obfd, osection);
      ASSERT(status, err);
    }
  
  /* --- create stub sections */
  if (verbose)
    printf(" defining stub sections\n");
  for (hsym=vtsyms; hsym; hsym=hsym->next)
    {
      asection *osection;
      asymbol *sym;
      char *name;
      /* new section */
      name = (char*)bfd_alloc(obfd, strlen(hsym->sym->name) + 28);
      ASSERT(name, enomem);
      strcpy(name,".gnu.linkonce.t.stub.");
      strcat(name,hsym->sym->name);
      hsym->relay = osection = bfd_make_section_anyway (obfd, name);
      ASSERT(osection, enomem);
      bfd_set_section_size(obfd, osection, stub_size);
      bfd_set_section_alignment(obfd, osection, stub_alignment);
      bfd_set_section_flags(obfd, osection, 
                            SEC_HAS_CONTENTS|SEC_ALLOC|SEC_LOAD|
                            SEC_RELOC|SEC_READONLY|SEC_CODE|SEC_LINK_ONCE);
    }
  
  /* --- copy symbols */
  bfd_set_symtab (obfd, isyms, isymcount);

  /* --- copy section relocs and data */
  if (verbose)
    printf(" copying section data from %s\n", iname);
  for (p=ibfd->sections; p; p=p->next)
    {
      const char *sname;
      const char *err = M("cannot copy data from section %s in file %s", 
                          bfd_section_name (ibfd, p), iname);
      asection *isection = p;
      asection *osection = p->output_section;
      bfd_size_type size = bfd_get_section_size_before_reloc (isection);
      /* copy relocs */
      reloc = (p->userdata) ? ((struct section_userdata*)isection->userdata)->reloc : 0;
      relcount = (p->userdata) ? ((struct section_userdata*)isection->userdata)->relcount : 0;
      sname = is_vt_section(ibfd, isection);
      if (sname)
        {
          if (verbose)
            printf("  patching relocs for %s\n", sname);
          for (i=0; i<relcount; i++)
            {
              arelent *rel = reloc[i];
              asymbol *sym = (rel->sym_ptr_ptr ? *rel->sym_ptr_ptr : 0);
              ASSERT(sym, eassert);
              hsym = lookup_symbol(&vtsymtab, sym->name, false);
              if (hsym)
                rel->sym_ptr_ptr = hsym->relay->symbol_ptr_ptr;
            }
        }
      bfd_set_reloc(obfd, osection, (relcount) ? reloc : 0, relcount);
      /* copy data */
      if (bfd_get_section_flags (ibfd, isection) & SEC_HAS_CONTENTS)
        {
          void *memchunk = malloc(size);
          ASSERT(memchunk, enomem);
          status = bfd_get_section_contents (ibfd, isection, memchunk, (file_ptr) 0, size);
          ASSERT(status, eassert);
          status = bfd_set_section_contents(obfd, osection, memchunk, (file_ptr) 0, size);
          ASSERT(status, eassert);
          free(memchunk);
        }
    }

  /* --- generate relocs and data for stub sections */
  for (hsym=vtsyms; hsym; hsym=hsym->next)
    {
      asection *osection = hsym->relay;
      (*stub_install)(obfd, osection, &hsym->sym);
    }
  
  /* --- copy private data */
  status = bfd_copy_private_bfd_data (ibfd, obfd);
  ASSERT(status, M("error copying private data for %s", iname));
  
  /* --- close bfds */
  if (verbose)
    printf(" finishing %s\n", iname);
  status = bfd_close(obfd);
  ASSERT(status, M("error while writing %s", oname));
  bfd_close(ibfd);
  
  /* --- cleanup */
  bfd_hash_table_free(&vtsymtab);
  return true;
}



/* Main */

int
main(int argc, char **argv)
{
  int i;
  int status = 0;
  program_name = strrchr(argv[0],'/');
  program_name = (program_name) ? program_name + 1 : argv[0];
  if (argc < 2)
    usage(stderr, 5);
  bfd_init();
  bfd_set_error_program_name(program_name);
  for (i=1; i<argc; i++)
    {
      if (!strcmp(argv[i], "-v"))
        verbose = 1;
      else if (!strcmp(argv[i], "-n"))
        replace = 0;
      else if (!strncmp(argv[i], "--target=", 9))
        target = argv[i] + 9;
      else
        {
          char *name = argv[i];
          char *newname = malloc(strlen(name) + 8);
          ASSERT(newname,enomem);
          strcpy(newname, name);
          strcat(newname, ".new");
          if (process(name, newname) && replace)
            {
              unlink(name);
              link(newname, name);
              unlink(newname);
            }
          free(newname);
        }
    }
  /* Finished */
  return status;
}


