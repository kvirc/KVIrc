#ifndef _MP_AMIPINTERFACE_H_
#define _MP_AMIPINTERFACE_H_

#include "kvi_settings.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

  #include "mp_interface.h"

  class KviAmipInterface : public KviMediaPlayerInterface
  {
  public:
    KviAmipInterface();
    ~KviAmipInterface();
  public:
    virtual int detect(bool bStart);
    virtual bool prev();
    virtual bool next();
    virtual bool play();
    virtual bool stop();
    virtual bool pause();
    virtual bool quit();
    virtual QString nowPlaying();
    virtual QString artist();
    virtual QString title();
    virtual QString genre();
    virtual QString comment();
    virtual QString album();
    virtual QString year();
    virtual QString mrl();
    virtual int position();
    virtual int length();
    virtual bool setVol(kvs_int_t &iVol);
    virtual int getVol();
    virtual bool jumpTo(kvs_int_t &iPos);
    virtual int sampleRate();
    virtual int bitRate();
    virtual int channels();
    virtual bool hide();
    virtual bool show();
    virtual bool minimize();
    virtual int getPlayListPos();
    virtual bool setPlayListPos(kvs_int_t &iPos);
    virtual int getListLength();
    virtual bool getRepeat();
    virtual bool getShuffle();
    virtual bool setRepeat(bool &bVal);
    virtual bool setShuffle(bool &bVal);
    virtual bool amipExec(const QString &cmd);
    virtual QString amipEval(const QString &cmd);
    virtual KviMediaPlayerInterface::PlayerStatus status();
  };
  
  MP_DECLARE_DESCRIPTOR(KviAmipInterface)
  
#endif //COMPILE_ON_WINDOWS

#endif //!_MP_AMIPINTERFACE_H_
