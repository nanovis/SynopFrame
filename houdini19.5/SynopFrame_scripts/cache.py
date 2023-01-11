import hou

def cache_all():
    hou.parm("/obj/All_NT/all_frame_with_nt_status/execute").pressButton()
    hou.parm("/obj/All_NT/mean_conf/execute").pressButton()
    hou.parm("/obj/PCA/PCA_cache/execute").pressButton()
    hou.parm("/obj/Schematic3D/design_cache/execute").pressButton()


def load_all():
    hou.parm("/obj/All_NT/all_frame_with_nt_status/reload").pressButton()
    hou.parm("/obj/All_NT/mean_conf/reload").pressButton()
    hou.parm("/obj/PCA/PCA_cache/reload").pressButton()
    hou.parm("/obj/Schematic3D/design_cache/reload").pressButton()
    