#include <iostream>

#include <xcb/xcb.h>
#include <xcb/render.h>
#include <cstdint>
#include <stdlib.h>
#include <string>
#include <iostream>

#define dump(pfix, n) do_dump((uint64_t)(pfix ## n), #pfix # n, # n);

void do_dump(uint64_t v, const char *label, const std::string &s)
{
	std::cout << "\t//! \\c " << label << std::endl << "\t";

	for (char c:s)
	{
		std::cout << (char)tolower(c);
	}

	std::cout << "=" << v << "," << std::endl;
}

int main()
{
	std::cout << "#ifndef DOXYGEN"
		  << std::endl << std::endl
		  << "//! Generated from xcb_render_pict_opt_t"
		  << std::endl << std::endl
		  << "enum class render_pict_op {" << std::endl;

	dump(XCB_RENDER_PICT_, OP_CLEAR);
	dump(XCB_RENDER_PICT_, OP_SRC);
	dump(XCB_RENDER_PICT_, OP_DST);
	dump(XCB_RENDER_PICT_, OP_OVER);
	dump(XCB_RENDER_PICT_, OP_OVER_REVERSE);
	dump(XCB_RENDER_PICT_, OP_IN);
	dump(XCB_RENDER_PICT_, OP_IN_REVERSE);
	dump(XCB_RENDER_PICT_, OP_OUT);
	dump(XCB_RENDER_PICT_, OP_OUT_REVERSE);
	dump(XCB_RENDER_PICT_, OP_ATOP);
	dump(XCB_RENDER_PICT_, OP_ATOP_REVERSE);
	dump(XCB_RENDER_PICT_, OP_XOR);
	dump(XCB_RENDER_PICT_, OP_ADD);
	dump(XCB_RENDER_PICT_, OP_SATURATE);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_CLEAR);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_SRC);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_DST);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_OVER);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_OVER_REVERSE);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_IN);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_IN_REVERSE);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_OUT);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_OUT_REVERSE);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_ATOP);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_ATOP_REVERSE);
	dump(XCB_RENDER_PICT_, OP_DISJOINT_XOR);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_CLEAR);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_SRC);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_DST);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_OVER);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_OVER_REVERSE);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_IN);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_IN_REVERSE);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_OUT);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_OUT_REVERSE);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_ATOP);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_ATOP_REVERSE);
	dump(XCB_RENDER_PICT_, OP_CONJOINT_XOR);
	dump(XCB_RENDER_PICT_, OP_MULTIPLY);
	dump(XCB_RENDER_PICT_, OP_SCREEN);
	dump(XCB_RENDER_PICT_, OP_OVERLAY);
	dump(XCB_RENDER_PICT_, OP_DARKEN);
	dump(XCB_RENDER_PICT_, OP_LIGHTEN);
	dump(XCB_RENDER_PICT_, OP_COLOR_DODGE);
	dump(XCB_RENDER_PICT_, OP_COLOR_BURN);
	dump(XCB_RENDER_PICT_, OP_HARD_LIGHT);
	dump(XCB_RENDER_PICT_, OP_SOFT_LIGHT);
	dump(XCB_RENDER_PICT_, OP_DIFFERENCE);
	dump(XCB_RENDER_PICT_, OP_EXCLUSION);
	dump(XCB_RENDER_PICT_, OP_HSL_HUE);
	dump(XCB_RENDER_PICT_, OP_HSL_SATURATION);
	dump(XCB_RENDER_PICT_, OP_HSL_COLOR);
	dump(XCB_RENDER_PICT_, OP_HSL_LUMINOSITY);


	std::cout << "};" << std::endl
		  << "//! Generated from xcb_render_repeat_t" << std::endl
		  << "enum class render_repeat {" << std::endl;

	dump(XCB_RENDER_REPEAT_, NONE);
	dump(XCB_RENDER_REPEAT_, NORMAL);
	dump(XCB_RENDER_REPEAT_, PAD);
	dump(XCB_RENDER_REPEAT_, REFLECT);
	std::cout << "};" << std::endl
		  << "//! Generated from xcb_join_style_t" << std::endl
		  << "enum class cap_style {" << std::endl;

	dump(XCB_CAP_STYLE_, NOT_LAST);
	dump(XCB_CAP_STYLE_, BUTT);
	dump(XCB_CAP_STYLE_, ROUND);
	dump(XCB_CAP_STYLE_, PROJECTING);
	std::cout << "};" << std::endl
		  << "//! Generated from xcb_join_style_t" << std::endl
		  << "enum class join_style {" << std::endl;

	dump(XCB_JOIN_STYLE_, MITER);
	dump(XCB_JOIN_STYLE_, ROUND);
	dump(XCB_JOIN_STYLE_, BEVEL);
	std::cout << "};" << std::endl
		  << "//! Generated from xcb_fill_style_t" << std::endl
		  << "enum class fill_rule {" << std::endl;

	dump(XCB_FILL_RULE_, EVEN_ODD);
	dump(XCB_FILL_RULE_, WINDING);
	std::cout << "};" << std::endl;
	std::cout << "#endif" << std::endl;
}
