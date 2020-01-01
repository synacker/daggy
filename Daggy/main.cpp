#include "Precompiled.h"

/*#include "Ssh2Client.h"
#include "Ssh2Process.h"

using namespace daggycore;*/

int main(int argc, char** argv) 
{
    QCoreApplication app(argc, argv);

    /*Ssh2Settings settings;
    Ssh2Client ssh2_client(settings);

    QObject::connect(&ssh2_client, &Ssh2Client::channelsCountChanged,
                     [&](int channels_count){
        if (channels_count == 0)
            ssh2_client.disconnectFromHost();
    });

    QObject::connect(&ssh2_client, &Ssh2Client::openChannelsCountChanged,
                     [&](int channels_count){
        qDebug() << "Open channels count: " << channels_count;
    });

    QObject::connect(&ssh2_client, &Ssh2Client::stateChanged,
                     [&](QAbstractSocket::SocketState state){
        qDebug() << "Scoket state: " << state;
    });

    QObject::connect(&ssh2_client, &Ssh2Client::sessionStateChanged,
                     [&](Ssh2Client::SessionStates state){
        qDebug() << "Ssh2 state: " << state;
        if (state == Ssh2Client::Connected) {
            Ssh2Process* ssh2_process = ssh2_client.createProcess("cat /home/muxa/.ssh/id_rsa");
            QObject::connect(ssh2_process, &Ssh2Process::channelStateChanged,
                    [=](Ssh2Channel::ChannelStates state) {
                switch (state) {
                case Ssh2Channel::Closed:
                case Ssh2Channel::FailedToOpen:
                    ssh2_process->deleteLater();
                    break;
                default:;
                }
            });
            QObject::connect(ssh2_process, &Ssh2Process::processStateChanged,
                    [=](Ssh2Process::ProcessStates state)
            {
                qDebug() << "Ssh2 process state: " << state;
                if (state == Ssh2Process::Finished) {
                    qDebug() << "Ssh2 process status: " << ssh2_process->exitStatus();
                    qDebug() << "Ssh2 process exit signal: " << ssh2_process->exitSignal();
                }
            }
            );
            QObject::connect(ssh2_process, &Ssh2Process::ssh2Error,
                    [](std::error_code error_code)
            {
                qDebug() << "Ssh2 debug process error: " << error_code.message().c_str();
            }
            );
            QObject::connect(ssh2_process, &Ssh2Process::channelReadyRead,
                    [=](int channel)
            {
                ssh2_process->setCurrentReadChannel(channel);
                qDebug() << "Ssh2 channel " << channel << " :";
                qDebug() << ssh2_process->readAll();
            }
            );
            ssh2_process->open();
        }
    });

    QObject::connect(&ssh2_client, &Ssh2Client::ssh2Error,
                     [](std::error_code error_code){
        qDebug() << "Ssh2 debug error: " << error_code.message().c_str();
    });

    ssh2_client.connectToHost("127.0.0.1", 22);*/

    return app.exec();
}
