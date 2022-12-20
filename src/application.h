#include <QObject>
#include <QtWidgets/QApplication>
#include <QSessionManager>

class Application : public QApplication
{
    Q_OBJECT
    public:
        Application(int &argc, char *argv[]);
        void commitData(QSessionManager &sessionManager);
//        void saveState(QSessionManager &sessionManager);
};
