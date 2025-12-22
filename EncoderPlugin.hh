#ifndef ENCODER_PLUGIN_HH
#define ENCODER_PLUGIN_HH

#include <gz/msgs.hh>
#include <gz/plugin/Register.hh>
#include <gz/sim/Joint.hh>
#include <gz/sim/Model.hh>
#include <gz/sim/System.hh>
#include <gz/transport/Node.hh>

#define ENCODER_RESOLUTION (10000)
#define GEAR_FACTOR (2.0f)

namespace gz::sim::systems
{
    class EncoderPlugin : public System, public ISystemConfigure, public ISystemPostUpdate
    {
    public:
        EncoderPlugin();
        ~EncoderPlugin() override = default;

        void Configure( const Entity                              &entity,
                        const std::shared_ptr<const sdf::Element> &sdf,
                        EntityComponentManager                    &ecm,
                        EventManager                              &eventMgr) override;


        void PostUpdate(const UpdateInfo &info, EntityComponentManager &ecm) ;
        void PostUpdate(const UpdateInfo &info, const EntityComponentManager &ecm) override;

    private:

        struct EncoderData
        {
            Entity      jointEntity;
            std::string jointName;
            double      lastPosition {0.0};
            int64_t     totalTicks {0};
            int64_t     ticksPerRevolution {ENCODER_RESOLUTION * ((int)GEAR_FACTOR)};
        };

        EncoderData m_leftEncoder;
        EncoderData m_rightEncoder;

        gz::transport::Node                             m_node;
        std::unique_ptr<gz::transport::Node::Publisher> m_encoderPub;
        std::string                                     m_topic {"/encoder_data"};

        Model m_model;
        bool  m_initialized {false};
    };
} // namespace gz::sim::systems

GZ_ADD_PLUGIN(
    gz::sim::systems::EncoderPlugin,
    gz::sim::System,
    gz::sim::systems::EncoderPlugin::ISystemConfigure,
    gz::sim::systems::EncoderPlugin::ISystemPostUpdate)

#endif