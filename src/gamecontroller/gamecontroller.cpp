#include "gamecontroller.h"

const QString GameController::xmlName = "gamecontroller";

GameController::GameController(SDL_GameController *controller, int deviceIndex, QObject *parent) :
    InputDevice(deviceIndex, parent)
{
    this->controller = controller;
    SDL_Joystick *joyhandle = SDL_GameControllerGetJoystick(controller);
    joystickID = SDL_JoystickInstanceID(joyhandle);

    for (int i=0; i < NUMBER_JOYSETS; i++)
    {
        GameControllerSet *controllerset = new GameControllerSet(this, i, this);
        joystick_sets.insert(i, controllerset);
        enableSetConnections(controllerset);
    }
}

QString GameController::getName()
{
    return QString(tr("GameController")).append(" ").append(QString::number(getRealJoyNumber()));
}

QString GameController::getSDLName()
{
    QString temp;
    if (controller)
    {
        temp = SDL_GameControllerName(controller);
    }

    return temp;
}

QString GameController::getGUIDString()
{
    QString temp;
    if (controller)
    {
        SDL_Joystick *joyhandle = SDL_GameControllerGetJoystick(controller);
        if (joyhandle)
        {
            SDL_JoystickGUID tempGUID = SDL_JoystickGetGUID(joyhandle);
            char guidString[65] = {'0'};
            SDL_JoystickGetGUIDString(tempGUID, guidString, sizeof(guidString));
            temp = QString(guidString);
        }
    }

    return temp;
}

QString GameController::getXmlName()
{
    return this->xmlName;
}

void GameController::closeSDLDevice()
{
    if (controller && SDL_GameControllerGetAttached(controller))
    {
        SDL_GameControllerClose(controller);
        controller = 0;
    }
}

int GameController::getNumberRawButtons()
{
    return SDL_CONTROLLER_BUTTON_MAX;
}

int GameController::getNumberRawAxes()
{
    return SDL_CONTROLLER_AXIS_MAX;
}

int GameController::getNumberRawHats()
{
    return 0;
}


void GameController::readConfig(QXmlStreamReader *xml)
{
    if (xml->isStartElement() && xml->name() == getXmlName())
    {
        reset();

        xml->readNextStartElement();
        while (!xml->atEnd() && (!xml->isEndElement() && xml->name() != getXmlName()))
        {
            if (xml->name() == "sets" && xml->isStartElement())
            {
                xml->readNextStartElement();

                while (!xml->atEnd() && (!xml->isEndElement() && xml->name() != "sets"))
                {
                    if (xml->name() == "set" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        index = index - 1;
                        if (index >= 0 && index < joystick_sets.size())
                        {
                            joystick_sets.value(index)->readConfig(xml);
                        }
                    }
                    else
                    {
                        // If none of the above, skip the element
                        xml->skipCurrentElement();
                    }

                    xml->readNextStartElement();
                }
            }
            else if (xml->name() == "names" && xml->isStartElement())
            {
                xml->readNextStartElement();
                while (!xml->atEnd() && (!xml->isEndElement() && xml->name() != "names"))
                {
                    if (xml->name() == "buttonname" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        QString temp = xml->readElementText();
                        index = index - 1;
                        if (index >= 0 && !temp.isEmpty())
                        {
                            setButtonName(index, temp);
                        }
                    }
                    else if (xml->name() == "triggerbuttonname" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        int buttonIndex = xml->attributes().value("button").toString().toInt();
                        QString temp = xml->readElementText();
                        index = (index - 1) + SDL_CONTROLLER_AXIS_TRIGGERLEFT;
                        buttonIndex = buttonIndex - 1;
                        if ((index == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
                             index == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) && !temp.isEmpty())
                        {
                            setAxisButtonName(index, buttonIndex, temp);
                        }
                    }
                    else if (xml->name() == "controlstickbuttonname" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        int buttonIndex = xml->attributes().value("button").toString().toInt();
                        QString temp = xml->readElementText();
                        index = index - 1;
                        if (index >= 0 && !temp.isEmpty())
                        {
                            setStickButtonName(index, buttonIndex, temp);
                        }
                    }
                    else if (xml->name() == "dpadbuttonname" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        int buttonIndex = xml->attributes().value("button").toString().toInt();
                        QString temp = xml->readElementText();
                        index = index - 1;
                        if (index >= 0 && !temp.isEmpty())
                        {
                            setVDPadButtonName(index, buttonIndex, temp);
                        }
                    }
                    else if (xml->name() == "triggername" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        QString temp = xml->readElementText();
                        index = (index - 1) + SDL_CONTROLLER_AXIS_TRIGGERLEFT;
                        if ((index == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
                             index == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) && !temp.isEmpty())
                        {
                            setAxisName(index, temp);
                        }
                    }
                    else if (xml->name() == "controlstickname" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        QString temp = xml->readElementText();
                        index = index - 1;
                        if (index >= 0 && !temp.isEmpty())
                        {
                            setStickName(index, temp);
                        }
                    }
                    else if (xml->name() == "dpadname" && xml->isStartElement())
                    {
                        int index = xml->attributes().value("index").toString().toInt();
                        QString temp = xml->readElementText();
                        index = index - 1;
                        if (index >= 0 && !temp.isEmpty())
                        {
                            setVDPadName(index, temp);
                        }
                    }
                    else
                    {
                        // If none of the above, skip the element
                        xml->skipCurrentElement();
                    }

                    xml->readNextStartElement();
                }
            }
            else
            {
                // If none of the above, skip the element
                xml->skipCurrentElement();
            }

            xml->readNextStartElement();
        }
    }
}

void GameController::writeConfig(QXmlStreamWriter *xml)
{
    xml->writeStartElement(getXmlName());
    xml->writeAttribute("configversion", QString::number(PadderCommon::LATESTCONFIGFILEVERSION));
    xml->writeAttribute("appversion", PadderCommon::programVersion);

    xml->writeComment("The SDL name for a joystick is included for informational purposes only.");
    xml->writeTextElement("sdlname", getSDLName());

    xml->writeStartElement("names"); // <names>

    SetJoystick *tempSet = getActiveSetJoystick();
    for (int i=0; i < getNumberButtons(); i++)
    {
        JoyButton *button = tempSet->getJoyButton(i);
        if (button && !button->getButtonName().isEmpty())
        {
            xml->writeStartElement("buttonname");
            xml->writeAttribute("index", QString::number(button->getRealJoyNumber()));
            xml->writeCharacters(button->getButtonName());
            xml->writeEndElement();
        }
    }

    for (int i=0; i < getNumberAxes(); i++)
    {
        JoyAxis *axis = tempSet->getJoyAxis(i);
        if (axis)
        {
            if (!axis->getAxisName().isEmpty())
            {
                xml->writeStartElement("axisname");
                xml->writeAttribute("index", QString::number(axis->getRealJoyIndex()));
                xml->writeCharacters(axis->getAxisName());
                xml->writeEndElement();
            }

            JoyAxisButton *naxisbutton = axis->getNAxisButton();
            if (!naxisbutton->getButtonName().isEmpty())
            {
                xml->writeStartElement("axisbuttonname");
                xml->writeAttribute("index", QString::number(axis->getRealJoyIndex()));
                xml->writeAttribute("button", QString::number(naxisbutton->getRealJoyNumber()));
                xml->writeCharacters(naxisbutton->getButtonName());
                xml->writeEndElement();
            }

            JoyAxisButton *paxisbutton = axis->getPAxisButton();
            if (!paxisbutton->getButtonName().isEmpty())
            {
                xml->writeStartElement("axisbuttonname");
                xml->writeAttribute("index", QString::number(axis->getRealJoyIndex()));
                xml->writeAttribute("button", QString::number(paxisbutton->getRealJoyNumber()));
                xml->writeCharacters(paxisbutton->getButtonName());
                xml->writeEndElement();
            }
        }
    }

    for (int i=0; i < getNumberSticks(); i++)
    {
        JoyControlStick *stick = tempSet->getJoyStick(i);
        if (stick)
        {
            if (!stick->getStickName().isEmpty())
            {
                xml->writeStartElement("controlstickname");
                xml->writeAttribute("index", QString::number(stick->getRealJoyIndex()));
                xml->writeCharacters(stick->getStickName());
                xml->writeEndElement();
            }

            QHash<JoyControlStick::JoyStickDirections, JoyControlStickButton*> *buttons = stick->getButtons();
            QHashIterator<JoyControlStick::JoyStickDirections, JoyControlStickButton*> iter(*buttons);
            while (iter.hasNext())
            {
                JoyControlStickButton *button = iter.next().value();
                if (button && !button->getButtonName().isEmpty())
                {
                    xml->writeStartElement("controlstickbuttonname");
                    xml->writeAttribute("index", QString::number(stick->getRealJoyIndex()));
                    xml->writeAttribute("button", QString::number(button->getRealJoyNumber()));
                    xml->writeCharacters(button->getButtonName());
                    xml->writeEndElement();
                }
            }
        }
    }

    for (int i=0; i < getNumberVDPads(); i++)
    {
        VDPad *vdpad = getActiveSetJoystick()->getVDPad(i);
        if (vdpad)
        {
            if (!vdpad->getDpadName().isEmpty())
            {
                xml->writeStartElement("dpadname");
                xml->writeAttribute("index", QString::number(vdpad->getRealJoyNumber()));
                xml->writeCharacters(vdpad->getDpadName());
                xml->writeEndElement();
            }

            QHash<int, JoyDPadButton*> *temp = vdpad->getButtons();
            QHashIterator<int, JoyDPadButton*> iter(*temp);
            while (iter.hasNext())
            {
                JoyDPadButton *button = iter.next().value();
                if (button && !button->getButtonName().isEmpty())
                {
                    xml->writeStartElement("dpadbutton");
                    xml->writeAttribute("index", QString::number(vdpad->getRealJoyNumber()));
                    xml->writeAttribute("button", QString::number(button->getRealJoyNumber()));
                    xml->writeCharacters(button->getButtonName());
                    xml->writeEndElement();
                }
            }
        }
    }
    xml->writeEndElement(); // </names>

    xml->writeStartElement("sets");
    for (int i=0; i < joystick_sets.size(); i++)
    {
        joystick_sets.value(i)->writeConfig(xml);
    }
    xml->writeEndElement();

    xml->writeEndElement();
}

QString GameController::getBindStringForAxis(int index)
{
    QString temp;
    SDL_GameControllerButtonBind bind = SDL_GameControllerGetBindForAxis(controller, (SDL_GameControllerAxis)index);
    if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE)
    {
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
        {
            temp.append("Button %1").arg(QString::number(bind.value.button));
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS)
        {
            temp.append("Axis %1").arg(QString::number(bind.value.axis));
        }
    }
    return temp;
}

QString GameController::getBindStringForButton(int index)
{
    QString temp;
    SDL_GameControllerButtonBind bind = SDL_GameControllerGetBindForButton(controller, (SDL_GameControllerButton)index);
    if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE)
    {
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
        {
            temp.append("Button %1").arg(QString::number(bind.value.button));
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS)
        {
            temp.append("Axis %1").arg(QString::number(bind.value.axis));
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_HAT)
        {
            temp.append("Hat %1.%2").arg(QString::number(bind.value.hat.hat))
                    .arg(QString::number(bind.value.hat.hat_mask));
        }
    }
    return temp;
}

SDL_GameControllerButtonBind GameController::getBindForAxis(int index)
{
    SDL_GameControllerButtonBind bind = SDL_GameControllerGetBindForAxis(controller, (SDL_GameControllerAxis)index);
    return bind;
}

SDL_GameControllerButtonBind GameController::getBindForButton(int index)
{
    SDL_GameControllerButtonBind bind = SDL_GameControllerGetBindForButton(controller, (SDL_GameControllerButton)index);
    return bind;
}

void GameController::buttonClickEvent(int buttonindex)
{
    SDL_GameControllerButtonBind bind = getBindForButton((SDL_GameControllerButton)buttonindex);
    if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE)
    {
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS)
        {
            //emit rawAxisButtonClick(bind.value.axis, 0);
            emit rawAxisActivated(bind.value.axis, JoyAxis::AXISMAX);
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
        {
            emit rawButtonClick(bind.value.button);
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_HAT)
        {
            emit rawDPadButtonClick(bind.value.hat.hat, bind.value.hat.hat_mask);
        }
    }
}

void GameController::buttonReleaseEvent(int buttonindex)
{
    SDL_GameControllerButtonBind bind = getBindForButton((SDL_GameControllerButton)buttonindex);
    if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE)
    {
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS)
        {
            emit rawAxisButtonRelease(bind.value.axis, 0);
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
        {
            emit rawButtonRelease(bind.value.button);
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_HAT)
        {
            emit rawDPadButtonRelease(bind.value.hat.hat, bind.value.hat.hat_mask);
        }
    }
}

void GameController::axisActivatedEvent(int setindex, int axisindex, int value)
{
    Q_UNUSED(setindex);

    SDL_GameControllerButtonBind bind = getBindForAxis((SDL_GameControllerButton)axisindex);
    if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE)
    {
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS)
        {
            //emit rawAxisButtonClick(bind.value.axis, 0);
            emit rawAxisActivated(bind.value.axis, value);
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
        {
            emit rawButtonClick(bind.value.button);
        }
        else if (bind.bindType == SDL_CONTROLLER_BINDTYPE_HAT)
        {
            emit rawDPadButtonClick(bind.value.hat.hat, bind.value.hat.hat_mask);
        }
    }
}

SDL_JoystickID GameController::getSDLJoystickID()
{
    return joystickID;
}
