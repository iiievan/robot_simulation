#ifndef ENCODER_PLUGIN_HH
#define ENCODER_PLUGIN_HH

#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/sim/Joint.hh>
#include <gz/transport/Node.hh>
#include <gz/msgs.hh>
#include <gz/plugin/Register.hh>

namespace gz::sim::systems
{
    class EncoderPlugin : public System,
                          public ISystemConfigure,
                          public ISystemPreUpdate
    {
    public:
        EncoderPlugin();
        ~EncoderPlugin() override = default;

        // Конфигурация системы
        void Configure(const Entity& entity,
                       const std::shared_ptr<const sdf::Element>& sdf,
                       EntityComponentManager& ecm,
                       EventManager& eventMgr) override;

        // Предварительное обновление системы
        void PreUpdate(const UpdateInfo& info,
                       EntityComponentManager& ecm) override;

    private:
        // Структура для хранения данных энкодера
        struct EncoderData
        {
            Entity jointEntity;
            std::string jointName;
            double lastPosition{0.0};
            int64_t totalTicks{0};
            int64_t ticksPerRevolution{20000};
        };

        EncoderData leftEncoder_;
        EncoderData rightEncoder_;

        gz::transport::Node node_;
        std::unique_ptr<gz::transport::Node::Publisher> encoderPub_;
        std::string topic_{"/encoder_data"};

        Model model_;
        bool initialized_{false};
    };
}



GZ_ADD_PLUGIN(
    gz::sim::systems::EncoderPlugin,
    gz::sim::System,
    gz::sim::systems::EncoderPlugin::ISystemConfigure,
    gz::sim::systems::EncoderPlugin::ISystemPreUpdate)

#endif