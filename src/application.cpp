#include "application.h"

Application::Application(int &argc, char *argv[]):QApplication(argc, argv)
{
}

void Application::commitData(QSessionManager &sessionManager)
{
    //    override default behaviour: sessionManager.cancel();
    sessionManager.release();
}

/*
void Application::saveState(QSessionManager &sessionManager)
{
    // Todo: Save history perhaps
}
*/
