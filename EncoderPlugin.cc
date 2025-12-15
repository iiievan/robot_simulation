#include "EncoderPlugin.hh"
#include <gz/common/Console.hh>
#include <gz/msgs/float_v.pb.h>
#include <gz/transport/Node.hh>
#include <gz/sim/components/JointPosition.hh>

using namespace gz;
using namespace sim;
using namespace systems;

// ДОБАВЬТЕ КОНСТРУКТОР:
EncoderPlugin::EncoderPlugin()
{
    gzmsg << "EncoderPlugin constructor called" << std::endl;
}

void EncoderPlugin::Configure(const Entity& entity,
                             const std::shared_ptr<const sdf::Element>& sdf,
                             EntityComponentManager& ecm,
                             EventManager& /*eventMgr*/)
{
    // Получаем модель
    model_ = Model(entity);
    if (!model_.Valid(ecm))
    {
        gzerr << "EncoderPlugin должен быть прикреплен к модели" << std::endl;
        return;
    }

    // Читаем параметры из SDF
    if (sdf->HasElement("left_joint"))
        leftEncoder_.jointName = sdf->Get<std::string>("left_joint");

    if (sdf->HasElement("right_joint"))
        rightEncoder_.jointName = sdf->Get<std::string>("right_joint");

    if (sdf->HasElement("topic"))
        topic_ = sdf->Get<std::string>("topic");

    if (sdf->HasElement("ticks_per_rev"))
    {
        int baseTicks = sdf->Get<int>("ticks_per_rev");
        leftEncoder_.ticksPerRevolution = baseTicks * 2;
        rightEncoder_.ticksPerRevolution = baseTicks * 2;
    }

    gzmsg << "EncoderPlugin инициализирован для модели: " << model_.Name(ecm) << std::endl;
    gzmsg << "Левый энкодер: " << leftEncoder_.jointName << std::endl;
    gzmsg << "Правый энкодер: " << rightEncoder_.jointName << std::endl;
    gzmsg << "Тиков на оборот колеса: " << leftEncoder_.ticksPerRevolution << std::endl;
}

void EncoderPlugin::PreUpdate(const UpdateInfo& info,
                             EntityComponentManager& ecm)
{
    if (info.paused)
        return;

    // Инициализация при первом обновлении
    if (!initialized_)
    {
        // Находим сущности шарниров
        leftEncoder_.jointEntity = model_.JointByName(ecm, leftEncoder_.jointName);
        rightEncoder_.jointEntity = model_.JointByName(ecm, rightEncoder_.jointName);

        if (leftEncoder_.jointEntity == kNullEntity ||
            rightEncoder_.jointEntity == kNullEntity)
        {
            gzerr << "Не удалось найти шарниры энкодеров" << std::endl;
            return;
        }

        // Создаем компоненты для шарниров, если их нет
        if (!ecm.EntityHasComponentType(leftEncoder_.jointEntity,
                                        components::JointPosition().TypeId()))
        {
            ecm.CreateComponent(leftEncoder_.jointEntity,
                               components::JointPosition({0.0}));
        }

        if (!ecm.EntityHasComponentType(rightEncoder_.jointEntity,
                                        components::JointPosition().TypeId()))
        {
            ecm.CreateComponent(rightEncoder_.jointEntity,
                               components::JointPosition({0.0}));
        }

        // Создаем публикатор
        encoderPub_ = std::make_unique<transport::Node::Publisher>(
            node_.Advertise<msgs::Float_V>(topic_));

        initialized_ = true;
        gzmsg << "EncoderPlugin полностью инициализирован" << std::endl;
    }

    // Получаем текущие позиции шарниров
    auto leftPosComp = ecm.Component<components::JointPosition>(leftEncoder_.jointEntity);
    auto rightPosComp = ecm.Component<components::JointPosition>(rightEncoder_.jointEntity);

    if (!leftPosComp || !rightPosComp)
        return;

    const std::vector<double>& leftPositions = leftPosComp->Data();
    const std::vector<double>& rightPositions = rightPosComp->Data();

    if (leftPositions.empty() || rightPositions.empty())
        return;

    double leftPos = leftPositions[0];
    double rightPos = rightPositions[0];

    // Вычисляем изменение позиции в радианах
    double leftDelta = leftPos - leftEncoder_.lastPosition;
    double rightDelta = rightPos - rightEncoder_.lastPosition;

    // Преобразуем радианы в тики энкодера
    int64_t leftTicks = static_cast<int64_t>((leftDelta / (2 * M_PI)) * leftEncoder_.ticksPerRevolution);
    int64_t rightTicks = static_cast<int64_t>((rightDelta / (2 * M_PI)) * rightEncoder_.ticksPerRevolution);

    // Обновляем общее количество тиков
    leftEncoder_.totalTicks += leftTicks;
    rightEncoder_.totalTicks += rightTicks;

    // Сохраняем текущие позиции
    leftEncoder_.lastPosition = leftPos;
    rightEncoder_.lastPosition = rightPos;

    // Публикуем данные
    msgs::Float_V encoderMsg;
    encoderMsg.add_data(static_cast<double>(leftEncoder_.totalTicks));
    encoderMsg.add_data(static_cast<double>(rightEncoder_.totalTicks));
    encoderMsg.add_data(static_cast<double>(leftTicks));
    encoderMsg.add_data(static_cast<double>(rightTicks));

    encoderPub_->Publish(encoderMsg);

    // Для отладки
    static int counter = 0;
    if (counter++ % 1000 == 0)
    {
        gzmsg << "Энкодеры - Левый: " << leftEncoder_.totalTicks
              << " тиков, Правый: " << rightEncoder_.totalTicks
              << " тиков" << std::endl;
    }
}