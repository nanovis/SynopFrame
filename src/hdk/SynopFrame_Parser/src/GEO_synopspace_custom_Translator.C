/*
 * Copyright (c) 2022
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 */

#include "GEO_synopspace_custom_Translator.h"

using namespace SynopSpace;

GEO_IOTranslator *
GEO_synopspace_custom_Translator::duplicate() const
{
    return new GEO_synopspace_custom_Translator(*this);
}

const char *
GEO_synopspace_custom_Translator::formatName() const
{
    return "Custom data for SynopSpace";
}

int
GEO_synopspace_custom_Translator::checkExtension(const char *name) 
{
    UT_String		sname(name);

	if (sname.fileExtension() && sname.endsWith(UT_String(".synopspace.custom")))
		return true;

    return false;
}

int
GEO_synopspace_custom_Translator::checkMagicNumber(unsigned magic)
{
    return 0;
}

GA_Detail::IOStatus
GEO_synopspace_custom_Translator::fileLoad(GEO_Detail *gdp, UT_IStream &is, bool ate_magic)
{
    // Convert our stream to ascii.
    UT_IStreamAutoBinary	forceascii(is, false);

	// Read first frame, parse box_sides

	// Read in frame_t
	if (!is.checkToken("t"))
		return GA_Detail::IOStatus(false);

	if (!is.checkToken("="))
		return GA_Detail::IOStatus(false);

	int frame_t;
	is.read(&frame_t);


	// ======= Read in the custom data ======= 
	is.getc(); // consume \n, the end line, for is.peek() to work properly

	constexpr int char_t = 116;
	unsigned int ptnum;
	while (is.peek() != char_t && !is.isEof())
	{
		// self-defined synopspace custom format
		float custom_data = -3;

		is.read<fpreal32>(&custom_data);


		ptnum = gdp->appendPoint();
		//gdp->setPos3(ptnum, UT_Vector3(0, 0, 0));


		GA_RWHandleI id(gdp->addIntTuple(GA_ATTRIB_POINT, "id", 1));
		id.set(gdp->pointOffset(ptnum), ptnum);

		GA_RWHandleI handle_custom_data(gdp->addIntTuple(GA_ATTRIB_POINT, "custom_data", 1));
		handle_custom_data.set(gdp->pointOffset(ptnum), custom_data);

		GA_RWHandleI t(gdp->addIntTuple(GA_ATTRIB_POINT, "t", 1));
		t.set(gdp->pointOffset(ptnum), frame_t);

		is.getc(); // consume \n, the end line, for is.peek() to work properly
	}


	// Read the rest frames, ignore box_sides
	while (is.checkToken("t"))
	{
		// Read in frame_t
		if (!is.checkToken("="))
			return GA_Detail::IOStatus(false);

		int frame_t;
		is.read(&frame_t);


		unsigned int ptnum = 0;
		unsigned int pt_id = 0;

		while (is.peek() != char_t && !is.isEof())
		{
			// self-defined synopspace custom format
			float custom_data = -3;

			is.read<fpreal32>(&custom_data);


			ptnum = gdp->appendPoint();
			//gdp->setPos3(ptnum, UT_Vector3(0, 0, 0));


			GA_RWHandleI id(gdp->addIntTuple(GA_ATTRIB_POINT, "id", 1));
			id.set(gdp->pointOffset(ptnum), pt_id++);

			GA_RWHandleI handle_custom_data(gdp->addIntTuple(GA_ATTRIB_POINT, "custom_data", 1));
			handle_custom_data.set(gdp->pointOffset(ptnum), custom_data);

			GA_RWHandleI t(gdp->addIntTuple(GA_ATTRIB_POINT, "t", 1));
			t.set(gdp->pointOffset(ptnum), frame_t);

			is.getc(); // consume \n, the end line, for is.peek() to work properly
		}
	}
	

    // All done successfully
    return GA_Detail::IOStatus(true);
}

GA_Detail::IOStatus
GEO_synopspace_custom_Translator::fileSave(const GEO_Detail *gdp, std::ostream &os)
{

    return GA_Detail::IOStatus(false);
}


