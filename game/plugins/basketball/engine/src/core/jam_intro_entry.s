.module jam_intro_entry

.include "vm.i"

.globl ___bank_scene_intro_01_court, _scene_intro_01_court

.area _CODE_255

___bank_jam_intro_entry = 255
.globl ___bank_jam_intro_entry

_jam_intro_entry::
        VM_RAISE EXCEPTION_CHANGE_SCENE, 3
        .db ___bank_scene_intro_01_court
        .dw _scene_intro_01_court
