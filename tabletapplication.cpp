
#include "tabletapplication.h"

bool TabletApplication::event(QEvent *event)
{
    if (event->type() == QEvent::TabletEnterProximity ||
        event->type() == QEvent::TabletLeaveProximity) {
        if (MyWindow != nullptr) {
            //  MyWindow->HandleTabletProximityEvent(static_cast<QTabletEvent *>(event));
        }
        return true;
    }
    return QApplication::event(event);
}
