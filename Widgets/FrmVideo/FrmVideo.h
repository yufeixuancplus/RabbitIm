#ifndef FRMVIDEO_H
#define FRMVIDEO_H

#include <QFrame>
#include <QCamera>
#include <QAudioOutput>
#include <QAudioInput>
#include <QBuffer>
#include <QTimer>
#include <QSound>
#include <QThread>
#include "CaptureVideoFrame.h"
#include "FrmPlayer.h"
#include "../../XmppClient.h"
#include "RecordAudio.h"

class CRoster;

namespace Ui {
class CFrmVideo;
}

class CFrmVideo : public QFrame
{
    Q_OBJECT

private:
    explicit CFrmVideo(QWidget *parent = 0);
    virtual ~CFrmVideo();
    int SetClient(CXmppClient *pClient);

public:
    static CFrmVideo *instance(CXmppClient *pClient);
    QThread* GetVideoThread();

    //主动发起呼叫
    int Call(QString jid);

protected slots:
    void clientIqReceived(const QXmppIq &iq);

    //有被呼叫时触发（只有被叫方才有）
    void callReceived(QXmppCall* pCall);
    //呼叫开始时触发(主叫方与被叫方都有)
    void callStarted(QXmppCall *pCall);

    //对方正在响铃时触发（只有呼叫方才有）
    void ringing();

    //呼叫状态改变时触发(主叫方与被叫方都有)
    void stateChanged(QXmppCall::State state);

    //呼叫会话建立时触发，这个是信令通道
    //当这个信号触发后,可以连接QAudioOutput和QAudioInput到呼叫
    //你可以调用通道的payloadType()来决定音频的参数（时钟、通道数等）
    //这时可以调用QXmppCall::startVideo开始视频
    void connected();

    //ICE连接建立后，音频信道有改变时触发，这个是数据通道
    void audioModeChanged(QIODevice::OpenMode mode);

    //ICE连接建立后，视频信道有改变时触发，这个是数据通道
    void videoModeChanged(QIODevice::OpenMode mode);

    //当呼叫结束时触摸发
    //注意：在这个函数中不能要直接删除呼叫(call)，call由qxmpp管理
    void finished();

private:
    void closeEvent(QCloseEvent *e);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *event);

    //连接与 call 相关的信号
    int ConnectionCallSlot(QXmppCall *pCall);
    
    //调整显示窗体大小
    void AdjustPlayer(const QRect &rect);

    int StartAudioDevice();
    int StopAudioDevice();
    int OpenAudioInput();
    int OpenAudioOutput();
    
    int StartVideo();
    int StopVideo();
    
    //播放拨号提示音
    void PlayCallSound(QXmppCall *pCall);
    void StopCallSound();

private:
    Ui::CFrmVideo *ui;

    QXmppCall* m_pCall;
    CXmppClient* m_pClient;

    QThread m_VideoThread;
    QThread m_AudioThread;

    QAudioInput*  m_pAudioInput; //音频输入设备
    QAudioOutput* m_pAudioOutput;//音频输出设备

    QCamera *m_pCamera;
    CCaptureVideoFrame m_CaptureVideoFrame;//实现捕获视频帧
    CFrmPlayer m_RemotePlayer;//远程视频播放窗口
    CFrmPlayer m_LocalePlayer;//本地视频播放窗口
    QTimer m_VideoPlayTimer;//显示对方视频定时器
    QList<QXmppVideoFrame> m_inFrames;
    QSound *m_pCallSound;//播放铃音

private slots:
    //用于向qxmpp输出视频幀
    void slotCaptureFrame(const QXmppVideoFrame &frame);
    //定时显示对方视频
    void slotUpdateReciverVideo();

};

#endif // FRMVIDEO_H
