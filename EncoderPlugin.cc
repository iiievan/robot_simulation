#include "EncoderPlugin.hh"
#include <gz/msgs/float_v.pb.h>
#include <gz/common/Console.hh>
#include <gz/sim/components/JointPosition.hh>
#include <gz/transport/Node.hh>
#include <cmath>

using namespace gz;
using namespace sim;
using namespace systems;

EncoderPlugin::EncoderPlugin()
{
    gzmsg << "EncoderPlugin constructor called" << std::endl;
}

void EncoderPlugin::Configure(const Entity                              &entity,
                              const std::shared_ptr<const sdf::Element> &sdf,
                              EntityComponentManager                    &ecm,
                              EventManager & /*eventMgr*/)
{
    // Get model
    m_model = Model(entity);
    if (!m_model.Valid(ecm))
    {
        gzerr << "EncoderPlugin must be attached to the model" << std::endl;
        return;
    }

    // Read SDF
    if (sdf->HasElement("left_joint"))
        m_leftEncoder.jointName = sdf->Get<std::string>("left_joint");

    if (sdf->HasElement("right_joint"))
        m_rightEncoder.jointName = sdf->Get<std::string>("right_joint");

    if (sdf->HasElement("topic"))
        m_topic = sdf->Get<std::string>("topic");

    if (sdf->HasElement("ticks_per_rev"))
    {
        int baseTicks = sdf->Get<int>("ticks_per_rev");
        m_leftEncoder.ticksPerRevolution = baseTicks * static_cast<int>(GEAR_FACTOR);
        m_rightEncoder.ticksPerRevolution = baseTicks * static_cast<int>(GEAR_FACTOR);
    }

    gzmsg << "EncoderPlugin initialized for model: " << m_model.Name(ecm) << std::endl;
    gzmsg << "Left encoder: " << m_leftEncoder.jointName << std::endl;
    gzmsg << "Right encoder: " << m_rightEncoder.jointName << std::endl;
    gzmsg << "Encoders resolution: " << m_leftEncoder.ticksPerRevolution << std::endl;
}

void EncoderPlugin::PostUpdate(const UpdateInfo &info, EntityComponentManager &ecm)
{
    if (info.paused)
        return;

    // Initialization on first update
    if (!m_initialized)
    {
        // Find joints
        m_leftEncoder.jointEntity = m_model.JointByName(ecm, m_leftEncoder.jointName);
        m_rightEncoder.jointEntity = m_model.JointByName(ecm, m_rightEncoder.jointName);

        if (m_leftEncoder.jointEntity == kNullEntity || m_rightEncoder.jointEntity == kNullEntity)
        {
            gzerr << "Unable to find encoder joints" << std::endl;
            return;
        }

        // В PostUpdate компоненты JointPosition уже созданы физическим движком,
        // поэтому создавать их не нужно. Просто проверяем их наличие.
        auto leftPosComp = ecm.Component<components::JointPosition>(m_leftEncoder.jointEntity);
        auto rightPosComp = ecm.Component<components::JointPosition>(m_rightEncoder.jointEntity);

        if (!leftPosComp || !rightPosComp)
        {
            gzerr << "JointPosition components not found for encoder joints" << std::endl;
            return;
        }

        // Инициализируем lastPosition текущими значениями
        const auto& leftPositions = leftPosComp->Data();
        const auto& rightPositions = rightPosComp->Data();

        if (!leftPositions.empty())
            m_leftEncoder.lastPosition = leftPositions[0];
        if (!rightPositions.empty())
            m_rightEncoder.lastPosition = rightPositions[0];

        // Create publisher
        m_encoderPub = std::make_unique<transport::Node::Publisher>(
            m_node.Advertise<msgs::Float_V>(m_topic));

        m_initialized = true;
        gzmsg << "EncoderPlugin fully initialized" << std::endl;
    }

    // Get the current positions of joints
    auto leftPosComp = ecm.Component<components::JointPosition>(m_leftEncoder.jointEntity);
    auto rightPosComp = ecm.Component<components::JointPosition>(m_rightEncoder.jointEntity);

    if (!leftPosComp || !rightPosComp)
        return;

    const std::vector<double> &leftPositions = leftPosComp->Data();
    const std::vector<double> &rightPositions = rightPosComp->Data();

    if (leftPositions.empty() || rightPositions.empty())
        return;

    double leftPos = leftPositions[0];
    double rightPos = rightPositions[0];

    // Calculate change in position in radians
    double leftDelta = leftPos - m_leftEncoder.lastPosition;
    double rightDelta = rightPos - m_rightEncoder.lastPosition;

    // Converting radians to encoder ticks
    int64_t leftTicks = static_cast<int64_t>((leftDelta / (2 * M_PI)) * m_leftEncoder.ticksPerRevolution);
    int64_t rightTicks = static_cast<int64_t>((rightDelta / (2 * M_PI)) * m_rightEncoder.ticksPerRevolution);

    // Updating the total number of ticks
    m_leftEncoder.totalTicks += leftTicks;
    m_rightEncoder.totalTicks += rightTicks;

    m_leftEncoder.lastPosition = leftPos;
    m_rightEncoder.lastPosition = rightPos;

    // Publish data
    msgs::Float_V encoderMsg;
    encoderMsg.add_data(static_cast<double>(m_leftEncoder.totalTicks));
    encoderMsg.add_data(static_cast<double>(m_rightEncoder.totalTicks));
    encoderMsg.add_data(static_cast<double>(leftTicks));
    encoderMsg.add_data(static_cast<double>(rightTicks));

    m_encoderPub->Publish(encoderMsg);

    // For debug
    static int counter = 0;
    if (counter++ % 1000 == 0)
    {
        gzmsg << "Энкодеры - Левый: " << m_leftEncoder.totalTicks << " тиков, Правый: " << m_rightEncoder.totalTicks
              << " тиков" << std::endl;
    }
}

void EncoderPlugin::PostUpdate(const UpdateInfo &info, const EntityComponentManager &ecm)
{
    // Просто вызываем версию с неконстантным параметром
    // Нужно снять const с ecm, так как нам нужно обновлять внутреннее состояние
    // В реальности Gazebo не будет вызывать эту версию, но мы должны её реализовать

    // Проверяем, что не в режиме паузы
    if (info.paused)
        return;

    // Если система уже инициализирована, нам нужно обновить тики
    if (m_initialized)
    {
        // Получаем текущие позиции (только чтение)
        auto leftPosComp = ecm.Component<components::JointPosition>(m_leftEncoder.jointEntity);
        auto rightPosComp = ecm.Component<components::JointPosition>(m_rightEncoder.jointEntity);

        if (!leftPosComp || !rightPosComp)
            return;

        const std::vector<double> &leftPositions = leftPosComp->Data();
        const std::vector<double> &rightPositions = rightPosComp->Data();

        if (leftPositions.empty() || rightPositions.empty())
            return;

        double leftPos = leftPositions[0];
        double rightPos = rightPositions[0];

        // Обновляем последние позиции (это изменение внутреннего состояния, а не ECM)
        m_leftEncoder.lastPosition = leftPos;
        m_rightEncoder.lastPosition = rightPos;
    }
}