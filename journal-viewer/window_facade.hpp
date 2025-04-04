#pragma once
#include <proxy/proxy.h>
namespace jrn
{

PRO_DEF_MEM_DISPATCH(MemDraw, draw);
PRO_DEF_MEM_DISPATCH(MemScrollToCursor, scroll_to_cursor);
struct LogWindowFacade :
    pro::facade_builder                                                //
    ::add_convention<MemDraw, void()>                                  //
    ::add_convention<MemScrollToCursor, void(std::string_view cursor)> //
    ::build
{};

using LogWindowComponent = pro::proxy<LogWindowFacade>;
} // namespace jrn
