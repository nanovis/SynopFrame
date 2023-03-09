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

#include "GEO_synopspace_hb_Translator.h"

using namespace SynopSpace;

GEO_IOTranslator *
GEO_synopspace_hb_Translator::duplicate() const
{
    return new GEO_synopspace_hb_Translator(*this);
}

const char *
GEO_synopspace_hb_Translator::formatName() const
{
    return "Hydrogen bond data for SynopSpace";
}

int
GEO_synopspace_hb_Translator::checkExtension(const char *name) 
{
    UT_String		sname(name);

	if (sname.fileExtension() && sname.endsWith(UT_String(".synopspace.hb")))
		return true;

    return false;
}

int
GEO_synopspace_hb_Translator::checkMagicNumber(unsigned magic)
{
    return 0;
}

GA_Detail::IOStatus
GEO_synopspace_hb_Translator::fileLoad(GEO_Detail *gdp, UT_IStream &is, bool ate_magic)
{
    // Convert our stream to ascii.
    UT_IStreamAutoBinary	forceascii(is, false);
	UT_WorkBuffer tmpBuffer;

	// Read first frame, parse box_sides

	// Read in frame_t
	if (!is.checkToken("t"))
		return GA_Detail::IOStatus(false);

	if (!is.checkToken("="))
		return GA_Detail::IOStatus(false);

	int frame_t;
	is.read(&frame_t);


	// ======= Read in the position data ======= 
	is.getc(); // consume \n, the end line, for is.peek() to work properly

	constexpr int char_t = 116;
	unsigned int ptnum;
	while (is.peek() != char_t && !is.isEof())
	{
		// self-defined synopspace hb format
		int nt_status = -3, pair_id = -1;

		is.read(&nt_status);

		if (nt_status == 2)
			is.read(&pair_id);


		ptnum = gdp->appendPoint();
		//gdp->setPos3(ptnum, UT_Vector3(0, 0, 0));


		GA_RWHandleI id(gdp->addIntTuple(GA_ATTRIB_POINT, "id", 1));
		id.set(gdp->pointOffset(ptnum), ptnum);

		GA_RWHandleI handle_nt_status(gdp->addIntTuple(GA_ATTRIB_POINT, "nt_status", 1));
		handle_nt_status.set(gdp->pointOffset(ptnum), nt_status);

		GA_RWHandleI handle_pair_id(gdp->addIntTuple(GA_ATTRIB_POINT, "pair_id", 1));
		handle_pair_id.set(gdp->pointOffset(ptnum), pair_id);

		GA_RWHandleI t(gdp->addIntTuple(GA_ATTRIB_POINT, "t", 1));
		t.set(gdp->pointOffset(ptnum), frame_t);

		is.getLine(tmpBuffer);
		//is.getc(); // consume \n, the end line, for is.peek() to work properly
		/*if (ptnum > 6537 && ptnum < 6542)
			std::cout << ptnum << ": " << tmpBuffer << ": " << is.peek() << std::endl;*/
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
			// self-defined synopspace hb format
			int nt_status = -3, pair_id = -1;

			is.read(&nt_status);

			if (nt_status == 2)
				is.read(&pair_id);


			ptnum = gdp->appendPoint();
			//gdp->setPos3(ptnum, UT_Vector3(0, 0, 0));


			GA_RWHandleI id(gdp->addIntTuple(GA_ATTRIB_POINT, "id", 1));
			id.set(gdp->pointOffset(ptnum), pt_id++);

			GA_RWHandleI handle_nt_status(gdp->addIntTuple(GA_ATTRIB_POINT, "nt_status", 1));
			handle_nt_status.set(gdp->pointOffset(ptnum), nt_status);

			GA_RWHandleI handle_pair_id(gdp->addIntTuple(GA_ATTRIB_POINT, "pair_id", 1));
			handle_pair_id.set(gdp->pointOffset(ptnum), pair_id);

			GA_RWHandleI t(gdp->addIntTuple(GA_ATTRIB_POINT, "t", 1));
			t.set(gdp->pointOffset(ptnum), frame_t);

			is.getLine(tmpBuffer); // consume \n, the end line, for is.peek() to work properly
		}
	}
	

    // All done successfully
    return GA_Detail::IOStatus(true);
}

GA_Detail::IOStatus
GEO_synopspace_hb_Translator::fileSave(const GEO_Detail *gdp, std::ostream &os)
{

    return GA_Detail::IOStatus(false);
}


