# robot_simulation

## Ёкспорт модели из blender в Collada .dae
```
¬кладка Main:
    [V] Selection Only
    [ ] Include Children
    [ ] Include Armatures  
    [ ] Include Shape Keys

    Global Orientation:
    [ ] Apply
    Forward Axis: [-Y]  <!-- ¬аш случай: пр€мо = -Y -->
    Up Axis: [Z]        <!-- ¬аш случай: вверх = Z -->

    Texture Options:
    [V] Copy
 UV [ ] Only Selected Map

¬кладка Geom:
    Export Data Options:
    [V] Triangulate
    Apply Modifiers [Viewport]  <!-- »ли Render если нужно -->
    Transform [Decomposed]      <!-- Ћучше дл€ Gazebo -->

¬кладка Arm:
    Armature Options: 
    [] Deform Bones only
    [] Export to SL/OpenSim

¬кладка Anim:
    [ ] Include Animations  <!-- ќтключите если нет анимации -->
    Key Type: [Samples]
    [ ] Keep Smooth Curves
    Sampling rate: {1}
    [ ] Keep Keyframes  
    [ ] All Keyed Curves
    [ ] Include all Actions
    Transform [Decomposed]

¬кладка Extra:
    Collada Options:
    [V] Use Object Instances
    [V] Use Blender Profile
    [] Sort By Object Name
    [] Keep Bind info
    [] Limit Precision

 где [] - пустое поле дл€ галочки
     [V] - пол€ в которых сто€т галочки по умолчанию.

```