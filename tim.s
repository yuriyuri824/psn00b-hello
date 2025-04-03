.section .data.tim_texture, "aw"
.balign 4

.global tim_texture
.type tim_texture, @object
tim_texture:
        .incbin "./texture.tim"
