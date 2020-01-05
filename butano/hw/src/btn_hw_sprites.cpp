#include "../include/btn_hw_sprites.h"

#include "tonc.h"
#include "btn_size.h"
#include "btn_memory.h"
#include "btn_sprite_shape_size.h"

namespace btn::hw::sprites
{

namespace
{
    static_assert(sizeof(OBJ_ATTR) == sizeof(handle));
    static_assert(alignof(OBJ_ATTR) == alignof(handle));

    handle* vram()
    {
        return reinterpret_cast<handle*>(MEM_OAM);
    }
}

void init()
{
    oam_init(reinterpret_cast<OBJ_ATTR*>(vram()), unsigned(available_sprites()));
}

void setup(sprite_shape shape, sprite_size size, int tile_id, int palette_id, bool eight_bits_per_pixel,
           int x, int y, int bg_priority, handle& sprite)
{
    auto sprite_ptr = reinterpret_cast<OBJ_ATTR*>(&sprite);
    int a0 = ATTR0_BUILD(0, int(shape), eight_bits_per_pixel, 0, 0, 0, 0);
    int a1 = ATTR1_BUILDR(0, int(size), 0, 0);
    int a2 = ATTR2_BUILD(tile_id, palette_id, bg_priority);
    obj_set_attr(sprite_ptr, uint16_t(a0), uint16_t(a1), uint16_t(a2));
    set_position(x, y, sprite);
}

size dimensions(const handle& sprite)
{
    auto sprite_ptr = reinterpret_cast<const OBJ_ATTR*>(&sprite);
    const uint8_t* obj_size = obj_get_size(sprite_ptr);
    size result(obj_size[0], obj_size[1]);

    if(sprite_ptr->attr0 & ATTR0_AFF_DBL_BIT)
    {
        result *= 2;
    }

    return result;
}

void set_tile(int tile_id, handle& sprite)
{
    auto sprite_ptr = reinterpret_cast<OBJ_ATTR*>(&sprite);
    BFN_SET(sprite_ptr->attr2, tile_id, ATTR2_ID);
}

void set_palette(int palette_id, handle& sprite)
{
    auto sprite_ptr = reinterpret_cast<OBJ_ATTR*>(&sprite);
    BFN_SET(sprite_ptr->attr2, palette_id, ATTR2_PALBANK);
}

void set_position(int x, int y, handle& sprite)
{
    auto sprite_ptr = reinterpret_cast<OBJ_ATTR*>(&sprite);
    size dims = dimensions(sprite);
    obj_set_pos(sprite_ptr, x - (dims.width() / 2), y - (dims.height() / 2));
}

void set_bg_priority(int bg_priority, handle& sprite)
{
    auto sprite_ptr = reinterpret_cast<OBJ_ATTR*>(&sprite);
    BFN_SET(sprite_ptr->attr2, bg_priority, ATTR2_PRIO);
}

void hide(handle& sprite)
{
    auto sprite_ptr = reinterpret_cast<OBJ_ATTR*>(&sprite);
    obj_hide(sprite_ptr);
}

void commit(const handle& sprites_ref, int count)
{
    memory::copy(sprites_ref, count, *vram());
}

}
