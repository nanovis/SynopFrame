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

#include <UT/UT_DSOVersion.h>

#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GEO/GEO_IOTranslator.h>
#include <SOP/SOP_Node.h>
#include <UT/UT_Assert.h>
#include <UT/UT_IOTable.h>

#include <iostream>
#include <stdio.h>

#include "GEO_synopspace_hb_Translator.h"
#include "GEO_synopspace_custom_Translator.h"


namespace SynopSpace {

class GEO_oxdna_dat_Translator : public GEO_IOTranslator
{
public:
	     GEO_oxdna_dat_Translator() {}
	     GEO_oxdna_dat_Translator(const GEO_oxdna_dat_Translator &src) {}
            ~GEO_oxdna_dat_Translator() override {}

    GEO_IOTranslator   *duplicate() const override;

    const char         *formatName() const override;

    int                 checkExtension(const char *name) override;

    int                 checkMagicNumber(unsigned magic) override;

    GA_Detail::IOStatus fileLoad(GEO_Detail *, UT_IStream &,
				 bool ate_magic) override;
    GA_Detail::IOStatus fileSave(const GEO_Detail *, std::ostream &) override;
};

}

using namespace SynopSpace;

GEO_IOTranslator *
GEO_oxdna_dat_Translator::duplicate() const
{
    return new GEO_oxdna_dat_Translator(*this);
}

const char *
GEO_oxdna_dat_Translator::formatName() const
{
    return "oxDNA Conf Dat Format";
}

int
GEO_oxdna_dat_Translator::checkExtension(const char *name) 
{
    UT_String		sname(name);

	if (sname.fileExtension() && sname.endsWith(UT_String(".oxdna.dat")))
		return true;

    return false;
}

int
GEO_oxdna_dat_Translator::checkMagicNumber(unsigned magic)
{
    return 0;
}

GA_Detail::IOStatus
GEO_oxdna_dat_Translator::fileLoad(GEO_Detail *gdp, UT_IStream &is, bool ate_magic)
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

	// Read in the box sides
	if (!is.checkToken("b"))
		return GA_Detail::IOStatus(false);

	if (!is.checkToken("="))
		return GA_Detail::IOStatus(false);

	float bx, by, bz;
	is.read<fpreal32>(&bx); is.read<fpreal32>(&by); is.read<fpreal32>(&bz);

	// Set the box sides
	GA_RWHandleV3 box_sides(gdp->addFloatTuple(GA_ATTRIB_DETAIL, "box_sides", 3));
	box_sides.set(GA_Offset(0), UT_Vector3(bx, by, bz));


	// Read in the Energy
	// Format doc is https://dna.physics.ox.ac.uk/index.php/Documentation#Configuration_file
	if (!is.checkToken("E"))
		return GA_Detail::IOStatus(false);
	if (!is.checkToken("="))
		return GA_Detail::IOStatus(false);

	float Etot, E_U, E_K;
	is.read<fpreal32>(&Etot); is.read<fpreal32>(&E_U); is.read<fpreal32>(&E_K);


	// ======= Read in the position data ======= 
	is.getc(); // consume \n, the end line, for is.peek() to work properly

	constexpr int char_t = 116;
	unsigned int ptnum;
	while (is.peek() != char_t && !is.isEof())
	{
		// oxDNA conf dat format
		// https://dna.physics.ox.ac.uk/index.php/Documentation#Configuration_and_topology_files
		float rx, ry, rz, bx, by, bz, nx, ny, nz, vx, vy, vz, Lx, Ly, Lz;

		is.read<fpreal32>(&rx);
		is.read<fpreal32>(&ry);
		is.read<fpreal32>(&rz);

		is.read<fpreal32>(&bx);
		is.read<fpreal32>(&by);
		is.read<fpreal32>(&bz);

		is.read<fpreal32>(&nx);
		is.read<fpreal32>(&ny);
		is.read<fpreal32>(&nz);

		is.read<fpreal32>(&vx);
		is.read<fpreal32>(&vy);
		is.read<fpreal32>(&vz);

		is.read<fpreal32>(&Lx);
		is.read<fpreal32>(&Ly);
		is.read<fpreal32>(&Lz);

		ptnum = gdp->appendPoint();
		gdp->setPos3(ptnum, UT_Vector3(rx, ry, rz));

		GA_RWHandleV3 a1(gdp->addFloatTuple(GA_ATTRIB_POINT, "a1", 3));
		a1.set(gdp->pointOffset(ptnum), UT_Vector3(bx, by, bz));

		GA_RWHandleV3 a3(gdp->addFloatTuple(GA_ATTRIB_POINT, "a3", 3));
		a3.set(gdp->pointOffset(ptnum), UT_Vector3(nx, ny, nz));

		GA_RWHandleV3 vel(gdp->addFloatTuple(GA_ATTRIB_POINT, "vel", 3));
		vel.set(gdp->pointOffset(ptnum), UT_Vector3(vx, vy, vz));

		GA_RWHandleV3 vel_ang(gdp->addFloatTuple(GA_ATTRIB_POINT, "vel_ang", 3));
		vel_ang.set(gdp->pointOffset(ptnum), UT_Vector3(Lx, Ly, Lz));

		GA_RWHandleI id(gdp->addIntTuple(GA_ATTRIB_POINT, "id", 1));
		id.set(gdp->pointOffset(ptnum), ptnum);

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

		// skip three lines: t line till end, the box sides, and the energy
		is.skipLine(); 
		is.skipLine();
		is.skipLine();

		
		unsigned int ptnum = 0;
		unsigned int pt_id = 0;

		while (is.peek() != char_t && !is.isEof())
		{
			// oxDNA conf dat format
			// https://dna.physics.ox.ac.uk/index.php/Documentation#Configuration_and_topology_files
			float rx, ry, rz, bx, by, bz, nx, ny, nz, vx, vy, vz, Lx, Ly, Lz;
			is.read<fpreal32>(&rx);
			is.read<fpreal32>(&ry);
			is.read<fpreal32>(&rz);

			is.read<fpreal32>(&bx);
			is.read<fpreal32>(&by);
			is.read<fpreal32>(&bz);

			is.read<fpreal32>(&nx);
			is.read<fpreal32>(&ny);
			is.read<fpreal32>(&nz);

			is.read<fpreal32>(&vx);
			is.read<fpreal32>(&vy);
			is.read<fpreal32>(&vz);

			is.read<fpreal32>(&Lx);
			is.read<fpreal32>(&Ly);
			is.read<fpreal32>(&Lz);

			ptnum = gdp->appendPoint();
			gdp->setPos3(ptnum, UT_Vector3(rx, ry, rz));

			GA_RWHandleV3 a1(gdp->addFloatTuple(GA_ATTRIB_POINT, "a1", 3));
			a1.set(gdp->pointOffset(ptnum), UT_Vector3(bx, by, bz));

			GA_RWHandleV3 a3(gdp->addFloatTuple(GA_ATTRIB_POINT, "a3", 3));
			a3.set(gdp->pointOffset(ptnum), UT_Vector3(nx, ny, nz));

			GA_RWHandleV3 vel(gdp->addFloatTuple(GA_ATTRIB_POINT, "vel", 3));
			vel.set(gdp->pointOffset(ptnum), UT_Vector3(vx, vy, vz));

			GA_RWHandleV3 vel_ang(gdp->addFloatTuple(GA_ATTRIB_POINT, "vel_ang", 3));
			vel_ang.set(gdp->pointOffset(ptnum), UT_Vector3(Lx, Ly, Lz));

			GA_RWHandleI id(gdp->addIntTuple(GA_ATTRIB_POINT, "id", 1));
			id.set(gdp->pointOffset(ptnum), pt_id++);

			GA_RWHandleI t(gdp->addIntTuple(GA_ATTRIB_POINT, "t", 1));
			t.set(gdp->pointOffset(ptnum), frame_t);


			is.getc(); // consume \n, the end line, for is.peek() to work properly
		}
	}
	

    // All done successfully
    return GA_Detail::IOStatus(true);
}

GA_Detail::IOStatus
GEO_oxdna_dat_Translator::fileSave(const GEO_Detail *gdp, std::ostream &os)
{

    return GA_Detail::IOStatus(false);
}

void
newGeometryIO(void *)
{
    GU_Detail::registerIOTranslator(new GEO_oxdna_dat_Translator());
	GU_Detail::registerIOTranslator(new GEO_synopspace_hb_Translator());
	GU_Detail::registerIOTranslator(new GEO_synopspace_custom_Translator());

    // Note due to the just-in-time loading of GeometryIO, the f3d
    // won't be added until after your first f3d save/load.
    // Thus this is replicated in the newDriverOperator.
    UT_ExtensionList		*geoextension;
    geoextension = UTgetGeoExtensions();
    if (!geoextension->findExtension("oxdna.dat"))
		geoextension->addExtension("oxdna.dat");

	if (!geoextension->findExtension("synopspace.hb"))
		geoextension->addExtension("synopspace.hb");

	if (!geoextension->findExtension("synopspace.custom"))
		geoextension->addExtension("synopspace.custom");
}
