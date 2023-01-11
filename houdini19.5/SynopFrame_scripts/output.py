import hou

def save_output():
    hou.node("/obj/Schematic3D/output_finalShape3D").cook(force=True)
    hou.node("/obj/Schematic2D/output_finalShape2D").cook(force=True)


def test_print():
    print("test\n")