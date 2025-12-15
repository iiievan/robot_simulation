# robot_simulation

## ������� ������ �� blender � Collada .dae
```
������� Main:
    [V] Selection Only
    [ ] Include Children
    [ ] Include Armatures  
    [ ] Include Shape Keys

    Global Orientation:
    [ ] Apply
    Forward Axis: [-Y]  <!-- ��� ������: ����� = -Y -->
    Up Axis: [Z]        <!-- ��� ������: ����� = Z -->

    Texture Options:
    [V] Copy
 UV [ ] Only Selected Map

������� Geom:
    Export Data Options:
    [V] Triangulate
    Apply Modifiers [Viewport]  <!-- ��� Render ���� ����� -->
    Transform [Decomposed]      <!-- ����� ��� Gazebo -->

������� Arm:
    Armature Options: 
    [] Deform Bones only
    [] Export to SL/OpenSim

������� Anim:
    [ ] Include Animations  <!-- ��������� ���� ��� �������� -->
    Key Type: [Samples]
    [ ] Keep Smooth Curves
    Sampling rate: {1}
    [ ] Keep Keyframes  
    [ ] All Keyed Curves
    [ ] Include all Actions
    Transform [Decomposed]

������� Extra:
    Collada Options:
    [V] Use Object Instances
    [V] Use Blender Profile
    [] Sort By Object Name
    [] Keep Bind info
    [] Limit Precision

 ��� [] - ������ ���� ��� �������
     [V] - ���� � ������� ����� ������� �� ���������.

```

To run from WSL2 and Ubuntu-24.04 
```
env -u WAYLAND_DISPLAY gz sim -v 4 esipovo.sdf
```