import hou



def update(pane_id, args):
    # print("callback test !! %d" % (pane_id))
    
    desktop = hou.ui.curDesktop()
    viewer_pane = desktop.panes()[pane_id]
    viewer = viewer_pane.tabOfType(hou.paneTabType.SceneViewer)
    
    viewer.setCurrentState("SynopSpace_xform", wait_for_exit=False, generate=hou.stateGenerateMode.Enter, request_new_on_generate=False, ex_situ_generate=False)

    viewer.runStateCommand("update_xform_handle", args)


def update_all():
    update(1, {"node": "/obj/Schematic3D/Schematic3D_CMS"})
    update(3, {"node": "/obj/Schematic2D/Schematic2D_CMS"})
    update(4, {"node": "/obj/Heatbar/Heatbar_CMS"})
    update(7, {"node": "/obj/All_NT/CMS_Position"})
    update(9, {"node": "/obj/Snakes/CMS_Snake_Prim"})


def update_frame():
    # only All_NT and Snakes are required
    update(7, {"node": "/obj/All_NT/CMS_Position"})
    update(9, {"node": "/obj/Snakes/CMS_Snake_Prim"})

    
# __import__("SynopSpace_xform_handle").update_all()