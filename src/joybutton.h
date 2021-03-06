#ifndef JOYBUTTON_H
#define JOYBUTTON_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QListIterator>
#include <QHash>
#include <QQueue>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "joybuttonslot.h"

class VDPad;
class SetJoystick;

class JoyButton : public QObject
{
    Q_OBJECT
public:
    explicit JoyButton(int index, int originset, SetJoystick *parentSet, QObject *parent=0);
    ~JoyButton();

    enum SetChangeCondition {SetChangeDisabled=0, SetChangeOneWay, SetChangeTwoWay, SetChangeWhileHeld};
    enum JoyMouseMovementMode {MouseCursor=0, MouseSpring};
    enum JoyMouseCurve {LinearCurve=0, QuadraticCurve, CubicCurve, QuadraticExtremeCurve, PowerCurve};

    void joyEvent (bool pressed, bool ignoresets=false);
    int getJoyNumber ();
    virtual int getRealJoyNumber ();
    void setJoyNumber (int index);

    bool getToggleState();
    int getTurboInterval();
    bool isUsingTurbo();
    void setCustomName(QString name);
    QString getCustomName();
    void setAssignedSlot(int code, JoyButtonSlot::JoySlotInputAction mode=JoyButtonSlot::JoyKeyboard);
    void setAssignedSlot(int code, int index, JoyButtonSlot::JoySlotInputAction mode=JoyButtonSlot::JoyKeyboard);
    void removeAssignedSlot(int index);

    QList<JoyButtonSlot*> *getAssignedSlots();

    virtual void readConfig(QXmlStreamReader *xml);
    virtual void writeConfig(QXmlStreamWriter *xml);

    virtual QString getPartialName(bool forceFullFormat=false, bool displayNames=false);
    virtual QString getSlotsSummary();
    virtual QString getSlotsString();
    virtual QString getName(bool forceFullFormat=false, bool displayNames=false);
    virtual QString getXmlName();

    int getMouseSpeedX();
    int getMouseSpeedY();

    int getWheelSpeedX();
    int getWheelSpeedY();

    void setChangeSetSelection(int index);
    int getSetSelection();

    virtual void setChangeSetCondition(SetChangeCondition condition, bool passive=false);
    SetChangeCondition getChangeSetCondition();

    bool getButtonState();
    int getOriginSet();

    bool containsSequence();
    bool containsDistanceSlots();
    bool containsReleaseSlots();

    virtual double getDistanceFromDeadZone();

    virtual void setVDPad(VDPad *vdpad);
    void removeVDPad();
    bool isPartVDPad();
    VDPad* getVDPad();

    virtual bool isDefault();
    void setIgnoreEventState(bool ignore);
    bool getIgnoreEventState();

    void setMouseMode(JoyMouseMovementMode mousemode);
    JoyMouseMovementMode getMouseMode();

    void setMouseCurve(JoyMouseCurve selectedCurve);
    JoyMouseCurve getMouseCurve();

    int getSpringWidth();
    int getSpringHeight();

    double getSensitivity();

    void setSmoothing(bool enabled=false);
    bool isSmoothingEnabled();
    bool getWhileHeldStatus();
    void setWhileHeldStatus(bool status);

    QString getActionName();
    QString getButtonName();

    virtual void setDefaultButtonName(QString tempname);
    virtual QString getDefaultButtonName();

    SetJoystick* getParentSet();

    static const QString xmlName;
    static const int ENABLEDTURBODEFAULT;
    static const double SMOOTHINGFACTOR;
    static const double DEFAULTMOUSESPEEDMOD;

protected:
    double getTotalSlotDistance(JoyButtonSlot *slot);
    bool distanceEvent();
    void clearAssignedSlots();
    void releaseSlotEvent();
    void findReleaseEventEnd();
    void findHoldEventEnd();
    bool checkForDelaySequence();
    void checkForPressedSetChange();

    // Used to denote whether the actual joypad button is pressed
    bool isButtonPressed;
    // Used to denote whether the virtual key is pressed
    bool isKeyPressed;
    bool toggle;
    bool quitEvent;
    // Used to denote the SDL index of the actual joypad button
    int index;
    int turboInterval;

    QTimer turboTimer;
    QTimer pauseTimer;
    QTimer holdTimer;
    QTimer pauseWaitTimer;
    QTimer createDeskTimer;
    QTimer releaseDeskTimer;
    QTimer mouseEventTimer;
    QTimer mouseWheelVerticalEventTimer;
    QTimer mouseWheelHorizontalEventTimer;
    QTimer setChangeTimer;
    QTimer keyDelayTimer;

    bool isDown;
    bool toggleActiveState;
    bool useTurbo;
    QList<JoyButtonSlot*> assignments;
    QList<JoyButtonSlot*> activeSlots;
    QString customName;

    int mouseSpeedX;
    int mouseSpeedY;
    int wheelSpeedX;
    int wheelSpeedY;

    int setSelection;
    SetChangeCondition setSelectionCondition;
    int originset;

    QListIterator<JoyButtonSlot*> *slotiter;
    JoyButtonSlot *currentPause;
    JoyButtonSlot *currentHold;
    JoyButtonSlot *currentCycle;
    JoyButtonSlot *previousCycle;
    JoyButtonSlot *currentDistance;
    JoyButtonSlot *currentMouseEvent;
    JoyButtonSlot *currentRelease;
    JoyButtonSlot *currentWheelVerticalEvent;
    JoyButtonSlot *currentWheelHorizontalEvent;

    bool ignoresets;
    QTime buttonHold;
    QTime pauseHold;
    QTime inpauseHold;
    QTime buttonHeldRelease;
    QTime keyDelayHold;

    QQueue<bool> ignoreSetQueue;
    QQueue<bool> isButtonPressedQueue;
    QQueue<JoyButtonSlot*> mouseEventQueue;
    QQueue<JoyButtonSlot*> mouseWheelVerticalEventQueue;
    QQueue<JoyButtonSlot*> mouseWheelHorizontalEventQueue;

    int currentRawValue;
    VDPad *vdpad;
    bool ignoreEvents;
    JoyMouseMovementMode mouseMode;
    JoyMouseCurve mouseCurve;

    int springWidth;
    int springHeight;
    double sensitivity;
    bool smoothing;
    bool whileHeldStatus;

    QString actionName;
    QString buttonName; // User specified button name
    QString defaultButtonName; // Name used by the system

    SetJoystick *parentSet; // Pointer to set that button is assigned to.

    unsigned int keyDelay;

    static double mouseSpeedModifier;
    static QList<JoyButtonSlot*> mouseSpeedModList;

    static QList<int> cursorXSpeeds;
    static QList<int> cursorYSpeeds;
    static QTimer cursorDelayTimer;

    static QList<double> springXSpeeds;
    static QList<double> springYSpeeds;
    static QTimer springDelayTimer;
    static QHash<unsigned int, int> activeKeys;
    static int globalKeyDelay;

signals:
    void clicked (int index);
    void released (int index);
    void keyChanged(int keycode);
    void mouseChanged(int mousecode);
    void setChangeActivated(int index);
    void setAssignmentChanged(int current_button, int associated_set, int mode);
    void finishedPause();
    void turboChanged(bool state);
    void toggleChanged(bool state);
    void turboIntervalChanged(int interval);
    void slotsChanged();
    void actionNameChanged();
    void buttonNameChanged();

public slots:
    void setTurboInterval (int interval);
    void setToggle (bool toggle);
    void setUseTurbo(bool useTurbo);
    void setMouseSpeedX(int speed);
    void setMouseSpeedY(int speed);

    void setWheelSpeedX(int speed);
    void setWheelSpeedY(int speed);

    void setSpringWidth(int value);
    void setSpringHeight(int value);

    void setSensitivity(double value);

    void setActionName(QString tempName);
    void setButtonName(QString tempName);

    virtual void reset();
    virtual void reset(int index);

    virtual void clearSlotsEventReset();

    void moveMouseCursor();
    void moveSpringMouse();

private slots:
    void turboEvent();
    virtual void mouseEvent();
    void createDeskEvent();
    void releaseDeskEvent(bool skipsetchange=false);
    void releaseActiveSlots();
    void activateSlots();
    void waitForDeskEvent();
    void waitForReleaseDeskEvent();
    void pauseEvent();
    void holdEvent();

    void wheelEventVertical();
    void wheelEventHorizontal();

    void pauseWaitEvent();
    void checkForSetChange();
    void keydelayEvent();
};


#endif // JOYBUTTON_H
