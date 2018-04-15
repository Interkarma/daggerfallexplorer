/*************************************************************************************************\
*
* Filename:		DFTypes.h
* Purpose:		Define structures for Daggerfall file management
* Version:		0.90
* Author:		Gavin Clayton
*
* Last Updated:	29/08/2002
*
* Copyright 2002. Gavin Clayton. All Rights Reserved.
*

*
* NOTE:
* This information is derived from Dave Humphrey's DF hacking articles (http://www.m0use.net/~uesp).
* I have occasionally used different naming conventions and expanded based on my own investigations.
* I am deeply grateful to Dave Humphrey for his work. This would not be possible without him.
*
* If changes are made by you to this code, please log them below:
*
*
\*************************************************************************************************/


#if !defined(_DFTYPES_H)
#define _DFTYPES_H


// Constant declarations
const NUM_TEXTURE_ARCHIVES = 469;
const NUM_IMG_FILES = 260;
const NUM_CIF_FILES = 70;
const NUM_RCI_FILES = 6;
const NUM_LOCALE_COUNT = 62;
const NUM_LOWER_ARCH3D_RECORD = 0;
const NUM_UPPER_ARCH3D_RECORD = 10250;
const NUM_LOWER_BLOCK_RECORD = 0;
const NUM_UPPER_BLOCK_RECORD = 1294;
const NUM_TEXTURE_PITCH = 256;
const NUM_CIF_PITCH = 320;
const NUM_MAX_CORNER_POINTS = 32;
const DTXSIZE_IMAGE_BUFFER = (256*3)*256;
const CIFSIZE_IMAGE_BUFFER = (320*3)*256;
const BLKSIZE_IMAGE_BUFFER = (64*3)*64;
const DTXSIZE_IMAGE_PITCH = 256*3;
const CIFSIZE_IMAGE_PITCH = 320*3;
const BLKSIZE_IMAGE_PITCH = 64*3;
const BSARECORD_LENGTH_SHORT = 8;
const BSARECORD_LENGTH_LONG = 18;


// Change struct packing to correctly reference byte-aligned DOS data
#pragma pack( push, defpack, 1 )


// Texture Archive Header
// A TEXTURE.nnn file is a single archive
typedef struct _tagTextureArchiveHeader
{
	short	nRecords;		// Total records in archive
	char	strName[24];	// Name of texture archive
} DTX_ARCHIVEHEADER, *LPDTX_ARCHIVEHEADER;

// Texture Record Directory
// There is one of these for each record in the archive
typedef struct _tagTextureRecordDirectory
{
	short	Unknown1;
	long	nOffset;		// Offset to record from start of archive
	char	Unknown2[14];	// Padding
} DTX_RECORDDIRECTORY, *LPDTX_RECORDDIRECTORY;

// Texture Record Header
// If nImages is 1, the image data is at nDataOffset
// Otherwise nDataOffset points to an image or row directory
typedef struct _tagTextureRecordHeader
{
	long	Unknown1;
	short	cx;				// Width of each image in the record
	short	cy;				// Height of each image in the record
	short	Compression;	// How the data is compressed
	long	nDataSize;		// Size of record data, including this header
	long	nDataOffset;	// Offset to record data from the start of this header
	short	Unknown2;
	short	nImages;		// Number of images in record
	char	Unknown3[6];	// Padding
} DTX_RECORDHEADER, *LPDTX_RECORDHEADER;

// Texture Image Directory
// There is one of these for each image in the record
typedef struct _tagTextureImageDirectory
{
	long	nOffset;		// Offset to image data from the start of the directory
} DTX_IMAGEDIRECTORY, *LPDTX_IMAGEDIRECTORY;

// Texture Image Data
typedef struct _tagTextureImageData
{
	short	cx;				// Width of this image
	short	cy;				// Height of this image
	char	Data;			// Start of image data
} DTX_IMAGEDATA, *LPDTX_IMAGEDATA;

// Texture RLE Row Directory
typedef struct _tagTextureRLERowDirectory
{
	short			nOffset;		// Offset to row from start of record header
	unsigned short	Encoded;		// 0x0000 if not encoded, 0x8000 if encoded
} DTX_RLEROWDIRECTORY, *LPDTX_RLEROWDIRECTORY;

// Texture RLE Row Data
typedef struct _tagTextureRLERowData
{
	short	nRowSize;		// Row width in pixels
	char	Data;			// Row Data
} DTX_RLEROWDATA, *LPDTX_RLEROWDATA;

// Texture RLE Row Chunk
typedef struct _tagTextureRLERowChunk
{
	signed short	nCount;			// Chunk RLE unspool count
	unsigned char	Data;			// RLE data
} DTX_RLEROWCHUNK, *LPDTX_RLEROWCHUNK;


// IMG Header
typedef struct _tagIMGHEADER
{
	long		Unknown1;
	short		cx;					// Width of this image
	short		cy;					// Height of this image
	short		Compression;		// Specifies if compression is enabled
	short		nImageSize;			// Actual size of image data in bytes
	char		Data;				// Image data
} IMG_HEADER, *LPIMG_HEADER;


// CIF Header
typedef struct _tagCIFWEAPONHEADER
{
	short		cx;					// Width of this image
	short		cy;					// Height of this image
	long		Unknown1;
	short		Unknown2;
	short		nImageSize;			// Actual size of image in bytes
	short		OffsetList[32];		// Offset from start of header to RLE image data
} CIF_WEAPONHEADER, *LPCIF_WEAPONHEADER;


// BSA Header
typedef struct _tagBSAHEADER
{
	short		nRecordCount;		// The number of records in this BSA archive
	short		nRecordType;		// The type of records in this BSA archive
} BSA_ARCHIVEHEADER, *LPBSA_ARCHIVEHEADER;

// BSA Long Directory entry
typedef struct _tagBSARecordDirectoryLong
{
	char		pszName[14];		// Name of this record
	long		nSize;				// Size of this record
} BSA_RECORDDIRECTORYLONG, *LPBSA_RECORDDIRECTORYLONG;

// BSA Short Directory entry
typedef struct _tagBSARecordDirectoryShort
{
	long		nID;				// ID of this record
	long		nSize;				// Size of this record
} BSA_RECORDDIRECTORYSHORT, *LPBSA_RECORDDIRECTORYSHORT;


// ARCH3D Header
typedef struct _tagARCH3DHeader
{
	char		szVersion[4];		// Version of object
	long		nPointCount;		// Number of points in point array
	long		nFaceCount;			// Number of faces in face array
	long		Unknown1;
	char		Unknown2[8];
	long		nOffset1;
	long		nOffset2;
	short		nOffset2Records;
	short		Unknown3;
	short		Unknown4;
	short		Unknown5;
	long		Unknown6;
	long		Unknown7;
	long		nPointOffset;		// Offset to point array?
	long		nNormalOffset;		// Offset to normal array?
	long		Unknown8;
	long		Unknown9;
} ARCH3D_HEADER, *LPARCH3D_HEADER;

// ARCH3D Point triplet
typedef struct _tagARCH3DPoint
{
	long		x, y, z;			// Point triplet
} ARCH3D_POINT, *LPARCH3D_POINT;

// ARCH3D Point Description
typedef struct _tagARCH3DPointDesc
{
	long		nOffset;			// Offset to point triplet
	short		tu;					// Texture U
	short		tv;					// Texture V
} ARCH3D_POINTDESC, *LPARCH3D_POINTDESC;

// ARCH3D Face definition
typedef struct _tagARCH3DFace
{
	char			nPointCount;		// Number of points in face
	char			Unknown1;
	unsigned short	nTexture;			// Bits 0-7 = Texture Record#; // Bits 8-15 = Texture Archive#
	short			Unknown2;
	short			Unknown3;
	char			Data;				// nPointCount sized array of ARCH3D_POINTDESC
} ARCH3D_FACE, *LPARCH3D_FACE;


// WOODS.WLD Header
typedef struct _tagWOODSWLDHeader
{
	long		nOffsetSize;		// Length in bytes of the data offsets section
	long		nWidth;				// Always 1000 - width of small map data image
	long		nHeight;			// Always 500 - height of small map data image
	long		Unknown1;
	long		nData1Offset;		// Offset from start of file to data section 1
	long		Unknown2;
	long		Unknown3;
	long		nSmMapOffset;		// Offset from start of file to small map data
	long		Unknown4[28];
	long		ChunkOffsets;		// Offsets to large map chunks
} WOODSWLD_HEADER, *LPWOODSWLD_HEADER;

// Large map data chunk
typedef struct _tagWOODSWLDLargeMapChunk
{
	short		Unknown1;
	long		Unknown2;
	short		nTextureArchive;	// Texture archive used in this chunk
	char		Unknown3;
	char		Unknown4;
	long		Unknown5[3];
	char		Data[5][5];			// Contains extended map data
} WOODSWLD_MAPCHUNK, *LPWOODSWLD_MAPCHUNK;


// RMBFLD Block Positions
typedef struct _RMBFLD_BLOCKPOS
{
	long		XPos1;				// Unknown XPos
	long		ZPos1;				// Unknown ZPos
	long		XPos2;				// XPos in map coords
	long		ZPos2;				// ZPos in map coords
	long		BlockRotation;		// Angle of block rotation (512 is 90 degrees of rotation on the Y axis, so 1024 is 180 degrees, etc.)
} RMBFLD_BLOCKPOS, *LPRMBFLD_BLOCKPOS;

// FMBFLD Filenames
typedef struct _RMBFLD_FILENAMES
{
	char		BlockFilename[13];	// Filename of this block
	char		Filenames[13][32];	// Other filenames?
} RMBFLD_FILENAMES, *LPRMBFLD_FILENAMES;

// RMB Small map data
typedef struct _RMBFLD_SMALLMAPDATA
{
	unsigned char	Header[8];					// Header?
	unsigned char	TextureInfo[16][16];		// Modified texture indices
	unsigned char	ObjectInfo[16][16];			// ?
} RMBFLD_SMALLMAPDATA, *LPRMBFLD_SMALLMAPDATA;

// RMB Fixed length data structure
typedef struct _RMBFLD
{
	char				nSubRecords1;		// SubRecord1 count
	char				nSubRecords2;		// SubRecord2 count
	char				nSubRecords3;		// SubRecord3 count
	RMBFLD_BLOCKPOS		BlockPos[32];		// Position of blocks
	char				Section1[832];		// What is this data?
	char				Section2[128];		// What is this data?
	long				Offsets[32];		// Record size of block data objects
	RMBFLD_SMALLMAPDATA	SmallMaps;			// Describes land under block
	char				Automap[4096];		// Automap pixel data 64x64
	RMBFLD_FILENAMES	Filenames;			// Block filenames
} RMBFLD, *LPRMBFLD;

// RMB Block header
typedef struct _RMB_BLOCKHEADER
{
	unsigned char		n3DObjectRecords;		// Number of 3D objects in this record
	unsigned char		nFlatObjectsRecords;	// Number of flat objects in this record
	unsigned char		nSection3Records;		// ?
	unsigned char		nPeopleRecords;			// Number of people in this record
	unsigned char		nDoorRecords;			// Number of doors in this record
	char				Unknown[12];			// ?
} RMB_BLOCKHEADER, *LPRMB_BLOCKHEADER;

// RMB 3D Objects Header
typedef struct _RMB_BLOCK3DOBJECTS
{
	short			nObjectID1;					// ObjectID1
	char			nObjectID2;					// ObjectID2 (ObjectID = ObjectID1 * 100 + ObjectID2)
	char			nObjectType;				// Object type {3=kibble?, ?}
	long			Unknown1;
	long			Unknown2;
	long			Unknown3;
	long			NullValue1;
	long			NullValue2;
	long			XPos1;						// ?
	long			YPos1;						// ?
	long			ZPos1;						// ?
	long			XPos2;						// X Offset
	long			YPos2;						// Y Offset
	long			ZPos2;						// Z Offset
	long			NullValue3;
	short			Rotation;					// Angle of object rotation (512 is 90 degrees of rotation on the Y axis, so 1024 is 180 degrees, etc.)?
	short			Unknown5;
	long			NullValue4;
	long			Unknown6;
	short			NullValue5;
} RMB_BLOCK3DOBJECTS, *LPRMB_BLOCK3DOBJECTS;

// RMB Flat Objects
typedef struct _RMB_BLOCKFLATOBJECTS
{
	char			Kludge[17];					// KLUDGE: Not implemented
} RMB_BLOCKFLATOBJECTS, *LPRMB_BLOCKFLATOBJECTS;

// RMB Data3
typedef struct _RMB_BLOCKDATA3
{
	long			XPos;
	long			YPos;
	long			ZPos;
	char			Unknown1;
	char			Unknown2;
	short			Unknown3;
} RMB_BLOCKDATA3, *LPRMB_BLOCKDATA3;

// RMB People Objects
typedef struct _RMB_BLOCKPEOPLEOBJECTS
{
	char			Kludge[17];					// KLUDGE: Not implemented
} RMB_BLOCKPEOPLEOBJECTS, *LPRMB_BLOCKPEOPLEOBJECTS;

// RMB Door Objects
typedef struct _RMB_BLOCKDOOROBJECTS
{
	char			Kludge[19];					// KLUDGE: Not implemented
} RMB_BLOCKDOOROBJECTS, *LPRMB_BLOCKDOOROBJECTS;


// Restore default packing
#pragma pack( pop, defpack )


// Supported Terrain Types
enum
{
	TERRAIN_DESERT		= 0,
	TERRAIN_MOUNTAIN	= 100,
	TERRAIN_TEMPERATE	= 300,
	TERRAIN_SWAMP		= 400,
	TERRAIN_NONE		= 1000,
};

// Supported weather types
enum
{
	WEATHER_NORMAL		= 0,
	WEATHER_WINTER		= 1,
	WEATHER_RAIN		= 2,
};

// Standard outdoor texture sets (support all terrains and +1 winter offset)
enum
{
	STO_TERRAIN			= 2,
	STO_RUINS			= 7,
	STO_CASTLE			= 9,
	STO_CITYA			= 12,
	STO_CITYB			= 14,
	STO_CITYWALLS		= 17,
	STO_FARM			= 26,
	STO_FENCES			= 29,
	STO_MAGESGUILD		= 35,
	STO_MANOR			= 38,
	STO_MERCHANTHOMES	= 42,
	STO_TAVERNEXTERIORS	= 58,
	STO_TEMPLEEXTERIORS	= 61,
	STO_VILLAGE			= 64,
	STO_ROOFS			= 69,
};


// Stores a Daggerfall image description for the Arena view
typedef struct _DFIMAGENAME
{
	char *Filename;
	char *Description;
} DFIMAGENAME;

// This array is missing: TEXTURE.215; TEXTURE.217; TEXTURE.436;
static DFIMAGENAME TextureArchives[] = {
	{"TEXTURE.000", "Solid Colors A"}, {"TEXTURE.001", "Solid Colors B"}, {"TEXTURE.002", "Desert terrain Test"},
	{"TEXTURE.003", "Desert Terrain Set Wint"}, {"TEXTURE.004", "Desert Terrain Rain"}, {"TEXTURE.005", "Steppes"},
	{"TEXTURE.006", "Mosque texs (Sentienel)"}, {"TEXTURE.007", "Desert Ruins"}, {"TEXTURE.008", "Desert Ruins"},
	{"TEXTURE.009", "Desert Castle"}, {"TEXTURE.010", "Desert Castle"}, {"TEXTURE.011", "Desert Palace Int."},
	{"TEXTURE.012", "Desert City A"}, {"TEXTURE.013", "Desert City A"}, {"TEXTURE.014", "Desert City B"},
	{"TEXTURE.015", "Desert City B"}, {"TEXTURE.016", "Desert City Int"}, {"TEXTURE.017", "Desert City Walls"},
	{"TEXTURE.018", "Desert City Walls"}, {"TEXTURE.019", "Desert Crypt A"}, {"TEXTURE.020", "Desert Crypt B"},
	{"TEXTURE.022", "Desert Dungeons A"}, {"TEXTURE.023", "Desert Dungeons B"}, {"TEXTURE.024", "Desert Dungeons C"},
	{"TEXTURE.025", "Desert Dungeons NEWCs"}, {"TEXTURE.026", "Desert Farm"}, {"TEXTURE.027", "Desert Farm"},
	{"TEXTURE.028", "Desert Farm Interiors"}, {"TEXTURE.029", "Desert Fences"}, {"TEXTURE.030", "Desert Fences"},
	{"TEXTURE.031", "roof top garden"}, {"TEXTURE.033", "Hedge textures"}, {"TEXTURE.035", "Desert Mages Guild"},
	{"TEXTURE.036", "Desert Mages Guild"}, {"TEXTURE.037", "Desert Mages Guild Int."}, {"TEXTURE.038", "Desert Manor"},
	{"TEXTURE.039", "Desert Manor"}, {"TEXTURE.040", "Desert Manor Interior"}, {"TEXTURE.041", "Desert Marble Floors"},
	{"TEXTURE.042", "Desert Merchant Homes"}, {"TEXTURE.043", "Desert Merchant Homes"}, {"TEXTURE.044", "Desert Merchant Home In"},
	{"TEXTURE.045", "Desert Mines"}, {"TEXTURE.046", "Desert Misc."}, {"TEXTURE.047", "Desert Natural Caves"},
	{"TEXTURE.048", "Desert Paintings"}, {"TEXTURE.049", "Generic Carpets"}, {"TEXTURE.050", "Ship Exteriors"},
	{"TEXTURE.053", "Signs"}, {"TEXTURE.054", "Woodland test set"}, {"TEXTURE.055", "Banners"},
	{"TEXTURE.056", "Dungeon Entrances"}, {"TEXTURE.057", "Tapestries"}, {"TEXTURE.058", "Tavern Exteriors"},
	{"TEXTURE.059", "Tavern Snow Exteriors"}, {"TEXTURE.060", "Tavern Interiors"}, {"TEXTURE.061", "Temple Exteriors"},
	{"TEXTURE.062", "Temple Snow Exteriors"}, {"TEXTURE.063", "Temple Interiors"}, {"TEXTURE.064", "Desert Village"},
	{"TEXTURE.065", "Desert Village"}, {"TEXTURE.066", "Desert Village Int."}, {"TEXTURE.067", "Misc Wood Planks"},
	{"TEXTURE.068", "Desert Sewer"}, {"TEXTURE.069", "Desert Roofs"}, {"TEXTURE.070", "Desert Roofs"},
	{"TEXTURE.071", "Desert Store Int"}, {"TEXTURE.072", "Desert Store Int B"}, {"TEXTURE.073", "Gravestone fronts"},
	{"TEXTURE.074", "Door Set"}, {"TEXTURE.075", "Various Gables"}, {"TEXTURE.076", "Various Sno Gables"},
	{"TEXTURE.077", "Various Other Gables"}, {"TEXTURE.079", "Woodland City Spec"}, {"TEXTURE.080", "Woodland City Spec Sno"},
	{"TEXTURE.081", "Desert Library Int"}, {"TEXTURE.082", "WoodCity Spec B"}, {"TEXTURE.083", "Woodcity Spec Sno"},
	{"TEXTURE.084", "test dragon skin"}, {"TEXTURE.085", "Base textures"}, {"TEXTURE.086", "Misc textures"},
	{"TEXTURE.087", "Fireplaces Winrak Silo"}, {"TEXTURE.088", "Wagon"}, {"TEXTURE.089", "HEDGE_TEXTURES"},
	{"TEXTURE.090", "MISC_FURNITURE"}, {"TEXTURE.091", "Mills"}, {"TEXTURE.092", "coffins organ"},
	{"TEXTURE.093", "Torture stuff"}, {"TEXTURE.094", "Misc Textures"}, {"TEXTURE.095", "Dungeon Exits"},
	{"TEXTURE.096", "CORPSES darkness cre"}, {"TEXTURE.097", "Statues"}, {"TEXTURE.098", "Statues of Monsters"},
	{"TEXTURE.099", "test gable"}, {"TEXTURE.100", "temp E3 Flats"}, {"TEXTURE.101", "temp E3 lights"},
	{"TEXTURE.102", "Mountain Terrain 7-5-96"}, {"TEXTURE.103", "Mountain Terrain Winter"}, {"TEXTURE.104", "Mountain Terrain Rain 7"},
	{"TEXTURE.105", "Underwater Flats"}, {"TEXTURE.106", "Underwater Flats (an"}, {"TEXTURE.107", "Mountain Ruins"},
	{"TEXTURE.108", "Mountain Ruins snow"}, {"TEXTURE.109", "Mountain Castle"}, {"TEXTURE.110", "Mountain Castle snow"},
	{"TEXTURE.111", "Mountain Palace Int."}, {"TEXTURE.112", "Mountain City A"}, {"TEXTURE.113", "Mountain City A snow"},
	{"TEXTURE.114", "Mountain City B"}, {"TEXTURE.115", "Mountain City B snow"}, {"TEXTURE.116", "Mountain City Int"},
	{"TEXTURE.117", "Mountain City Walls"}, {"TEXTURE.118", "Mountain City Walls sno"}, {"TEXTURE.119", "Mountain Crypt A"},
	{"TEXTURE.120", "Mountain Crypt B"}, {"TEXTURE.121", "Arches and doorways"}, {"TEXTURE.122", "Mountain Dungeons A"},
	{"TEXTURE.123", "Mountain Dungeons B"}, {"TEXTURE.124", "Mountain Dungeons C"}, {"TEXTURE.125", "Mountain Dungeons NEWCs"},
	{"TEXTURE.126", "Mountain Farm"}, {"TEXTURE.127", "Mountain Farm snow"}, {"TEXTURE.128", "Mountain Farm Interiors"},
	{"TEXTURE.129", "Mountain Fences snow"}, {"TEXTURE.130", "Mountain Fences"}, {"TEXTURE.131", "Dineri tower ext"},
	{"TEXTURE.132", "Dineri tower int"}, {"TEXTURE.133", "Llugwych ext"}, {"TEXTURE.134", "Llugwych int"},
	{"TEXTURE.135", "Mountain Mages Guild"}, {"TEXTURE.136", "Mountain Mages Guild sn"}, {"TEXTURE.137", "Mountain Mages Guild In"},
	{"TEXTURE.138", "Mountain Manor"}, {"TEXTURE.139", "Mountain Manor snow"}, {"TEXTURE.140", "Mountain Manor Interior"},
	{"TEXTURE.141", "Mountain Marble Floors"}, {"TEXTURE.142", "Mountain Merchant Homes"}, {"TEXTURE.143", "Mountain Merchant Homes"},
	{"TEXTURE.144", "Mountain Merchant Home"}, {"TEXTURE.145", "Mountain Mines"}, {"TEXTURE.146", "Archway textures"},
	{"TEXTURE.147", "Mountain Natural Caves"}, {"TEXTURE.148", "Mountain Paintings"}, {"TEXTURE.149", "shedungent ext"},
	{"TEXTURE.150", "Shedungent int"}, {"TEXTURE.151", "Woodborne Hall ext"}, {"TEXTURE.152", "Woodborne Hall int"},
	{"TEXTURE.153", "Lysandus's tomb"}, {"TEXTURE.154", "Orsinium ext"}, {"TEXTURE.155", "Orsinium int"},
	{"TEXTURE.156", "Scourge ext"}, {"TEXTURE.157", "Scourge int"}, {"TEXTURE.158", "Mountain Tavern"},
	{"TEXTURE.159", "Mountain Tavern Snow"}, {"TEXTURE.160", "Mountain Tavern Int."}, {"TEXTURE.161", "Mountain Temple"},
	{"TEXTURE.162", "Mountain Temple snow"}, {"TEXTURE.163", "Mountain Temple Int."}, {"TEXTURE.164", "Mountain Village"},
	{"TEXTURE.165", "Mountain Village Snow"}, {"TEXTURE.166", "Mountain Village Int."}, {"TEXTURE.167", "Archway textures II"},
	{"TEXTURE.168", "Mountain Sewer"}, {"TEXTURE.169", "Mountain Roofs"}, {"TEXTURE.170", "Mountain Roofs snow"},
	{"TEXTURE.171", "Barn interior"}, {"TEXTURE.172", "Barn Exterior"}, {"TEXTURE.173", "Barn Ext/Snow"},
	{"TEXTURE.174", "Door Set"}, {"TEXTURE.175", "Daedra Lords"}, {"TEXTURE.176", "Dark Brotherhood"},
	{"TEXTURE.177", "Mages"}, {"TEXTURE.178", "Necromancers"}, {"TEXTURE.179", "Witches Coven"},
	{"TEXTURE.180", "Courtiers"}, {"TEXTURE.181", "Temple"}, {"TEXTURE.182", "medium Common people"},
	{"TEXTURE.183", "Noble"}, {"TEXTURE.184", "Flat People II"}, {"TEXTURE.185", "test noble replace"},
	{"TEXTURE.186", "Test Big Flats"}, {"TEXTURE.190", "LIGHTS"}, {"TEXTURE.194", "Kludge Swamp"},
	{"TEXTURE.195", "Kludge Town"}, {"TEXTURE.197", "Kludge Town"}, {"TEXTURE.198", "Kludge Art"},
	{"TEXTURE.199", "Editor Requred Flats"}, {"TEXTURE.200", "House Furnishings"}, {"TEXTURE.201", "Animals"},
	{"TEXTURE.202", "Art"}, {"TEXTURE.203", "Buildings"}, {"TEXTURE.204", "Clothing"},
	{"TEXTURE.205", "Container"}, {"TEXTURE.206", "Death"}, {"TEXTURE.207", "Equipment"},
	{"TEXTURE.208", "Exotica"}, {"TEXTURE.209", "Library"}, {"TEXTURE.210", "Lights"},
	{"TEXTURE.211", "Misc"}, {"TEXTURE.212", "Outdoor"}, {"TEXTURE.213", "Plants"},
	{"TEXTURE.214", "Tools"}, {"TEXTURE.216", "Treasure"}, {"TEXTURE.218", "Pots and Pans"},
	{"TEXTURE.233", ""}, {"TEXTURE.234", ""}, {"TEXTURE.235", ""},
	{"TEXTURE.236", ""}, {"TEXTURE.237", ""}, {"TEXTURE.238", ""},
	{"TEXTURE.239", ""}, {"TEXTURE.240", ""}, {"TEXTURE.241", ""},
	{"TEXTURE.242", ""}, {"TEXTURE.245", ""}, {"TEXTURE.246", ""},
	{"TEXTURE.247", ""}, {"TEXTURE.248", ""}, {"TEXTURE.249", ""},
	{"TEXTURE.250", ""}, {"TEXTURE.251", ""}, {"TEXTURE.252", ""},
	{"TEXTURE.253", ""}, {"TEXTURE.254", ""}, {"TEXTURE.255", "Rat"},
	{"TEXTURE.256", "Imp"}, {"TEXTURE.257", "Spriggan"}, {"TEXTURE.258", "Giant Bat"},
	{"TEXTURE.259", "Bear"}, {"TEXTURE.260", "Tiger"}, {"TEXTURE.261", "Spider"},
	{"TEXTURE.262", "Orc"}, {"TEXTURE.263", "Centaur"}, {"TEXTURE.264", "Werewolf"},
	{"TEXTURE.265", "Nymph"}, {"TEXTURE.266", "Eel"}, {"TEXTURE.267", "Orc Sergeant"},
	{"TEXTURE.268", "Harpy"}, {"TEXTURE.269", "Wereboar"}, {"TEXTURE.270", "Skeleton"},
	{"TEXTURE.271", "Giant"}, {"TEXTURE.272", "Zombie"}, {"TEXTURE.273", "Ghost"},
	{"TEXTURE.274", "Mummy"}, {"TEXTURE.275", "Scorpion"}, {"TEXTURE.276", "Orc Shaman"},
	{"TEXTURE.277", "Gargoyle"}, {"TEXTURE.278", "Wraith"}, {"TEXTURE.279", "Orc Warlord"},
	{"TEXTURE.280", "Frost Daedra"}, {"TEXTURE.281", "Fire Daedra"}, {"TEXTURE.282", "Lesser Daedra"},
	{"TEXTURE.283", "Female vampire"}, {"TEXTURE.284", "Seducer"}, {"TEXTURE.285", "Vampire"},
	{"TEXTURE.286", "Daedra lord"}, {"TEXTURE.287", "Lich"}, {"TEXTURE.288", "Lich King"},
	{"TEXTURE.289", "Faery Dragon"}, {"TEXTURE.290", "Fire atronach"}, {"TEXTURE.291", "Iron atronach"},
	{"TEXTURE.292", "Flesh atronach"}, {"TEXTURE.293", "Ice atronach"}, {"TEXTURE.295", "Faery Dragon"},
	{"TEXTURE.296", "Dreugh"}, {"TEXTURE.297", "Lamia"}, {"TEXTURE.298", "specials"},
	{"TEXTURE.299", "Female vampire"}, {"TEXTURE.300", "cave formations"}, {"TEXTURE.301", "Crops"},
	{"TEXTURE.302", "Temperate Terrain Set"}, {"TEXTURE.303", "Woodland Terrain Snow"}, {"TEXTURE.304", "Woodland Rain 2-21"},
	{"TEXTURE.305", "CORPSES WATER MONSTE"}, {"TEXTURE.306", "CORPSES undead"}, {"TEXTURE.307", "Woodland Ruins"},
	{"TEXTURE.308", "Woodland Ruins snow"}, {"TEXTURE.309", "Woodland Castle"}, {"TEXTURE.310", "Woodland Castle"},
	{"TEXTURE.311", "Woodland Palace Int."}, {"TEXTURE.312", "Woodland City A"}, {"TEXTURE.313", "Woodland City A snow"},
	{"TEXTURE.314", "Woodland City B"}, {"TEXTURE.315", "Woodland City B snow"}, {"TEXTURE.316", "Woodland City Int"},
	{"TEXTURE.317", "Woodland City Walls"}, {"TEXTURE.318", "Woodland City Walls"}, {"TEXTURE.319", "Woodland Crypt A"},
	{"TEXTURE.320", "Woodland Crypt B"}, {"TEXTURE.321", "Arches and doorways VI"}, {"TEXTURE.322", "Woodland Dungeons A"},
	{"TEXTURE.323", "Woodland Dungeons B"}, {"TEXTURE.324", "Woodland Dungeons C"}, {"TEXTURE.325", "Woodland Dungeons NEWCs"},
	{"TEXTURE.326", "Woodland Farm"}, {"TEXTURE.327", "Woodland Farm snow"}, {"TEXTURE.328", "Woodland Farm Interiors"},
	{"TEXTURE.329", "Woodland Fences snow"}, {"TEXTURE.330", "Woodland Fences"}, {"TEXTURE.331", "Dungeon Entrances(ruin)"},
	{"TEXTURE.332", "Temp Big Doors"}, {"TEXTURE.333", "Store/tavern Signs"}, {"TEXTURE.334", "Daggerfall people"},
	{"TEXTURE.335", "Woodland Mages Guild"}, {"TEXTURE.336", "Woodland Mages Guild sn"}, {"TEXTURE.337", "Woodland Mages Guild In"},
	{"TEXTURE.338", "Woodland Manor"}, {"TEXTURE.339", "Woodland Manor snow"}, {"TEXTURE.340", "Woodland Manor Interior"},
	{"TEXTURE.341", "Woodland Marble Floors"}, {"TEXTURE.342", "Woodland Merchant Homes"}, {"TEXTURE.343", "Woodland Merchant Homes"},
	{"TEXTURE.344", "Woodland Merchant Home"}, {"TEXTURE.345", "Woodland Mines"}, {"TEXTURE.346", "Wayrest Flat people"},
	{"TEXTURE.347", "Woodland  Natural Caves"}, {"TEXTURE.348", "Woodland Paintings"}, {"TEXTURE.349", "Daggerfall ext"},
	{"TEXTURE.350", "Daggerfall int"}, {"TEXTURE.351", "Wayrest ext"}, {"TEXTURE.352", "Wayrest int"},
	{"TEXTURE.353", "Sentinel ext"}, {"TEXTURE.354", "Sentinel int"}, {"TEXTURE.355", "Mantellan Crux int"},
	{"TEXTURE.356", "Mantellan Crux(fire) in"}, {"TEXTURE.357", "Sentienel flat people"}, {"TEXTURE.358", "Tavern Exteriors"},
	{"TEXTURE.359", "Tavern Snow Exteriors"}, {"TEXTURE.360", "Tavern Interiors"}, {"TEXTURE.361", "Temple Exteriors"},
	{"TEXTURE.362", "Temple Snow Exteriors"}, {"TEXTURE.363", "Temple Interiors"}, {"TEXTURE.364", "Woodland Village"},
	{"TEXTURE.365", "Woodland Village snow"}, {"TEXTURE.366", "Woodland Village Int."}, {"TEXTURE.368", "Woodland Sewer"},
	{"TEXTURE.369", "Woodland Roofs"}, {"TEXTURE.370", "Woodland Roofs"}, {"TEXTURE.371", "Misc Mantella texs"},
	{"TEXTURE.372", "X-tra Marbles"}, {"TEXTURE.374", "Door Set"}, {"TEXTURE.375", "Fire Spell Effects"},
	{"TEXTURE.376", "Frost Spell Effects"}, {"TEXTURE.377", "Poison Spell Effects"}, {"TEXTURE.378", "Shock Spell Effects"},
	{"TEXTURE.379", "Magic Spell Effects"}, {"TEXTURE.380", "Blood and Gore+"}, {"TEXTURE.381", "male redguard"},
	{"TEXTURE.382", "male redguard"}, {"TEXTURE.383", "male redguard"}, {"TEXTURE.384", "male redguard"},
	{"TEXTURE.385", "male breton"}, {"TEXTURE.386", "MALE NORD"}, {"TEXTURE.387", "MALE NORD"},
	{"TEXTURE.388", "MALE NORD"}, {"TEXTURE.389", "MALE NORD"}, {"TEXTURE.390", "MALE NORD"},
	{"TEXTURE.391", "monc"}, {"TEXTURE.392", "female nord"}, {"TEXTURE.393", "female nord"},
	{"TEXTURE.394", "sailor"}, {"TEXTURE.395", "redguard female"}, {"TEXTURE.396", "redguard female"},
	{"TEXTURE.397", "REDGUARD WOMAN"}, {"TEXTURE.398", "REDGUARD WOMAN"}, {"TEXTURE.399", "Guard"},
	{"TEXTURE.400", "CORPSES daedra"}, {"TEXTURE.401", "CORPSES animals"}, {"TEXTURE.402", "swamp Terrain Set"},
	{"TEXTURE.403", "Swamp Terrain Winter"}, {"TEXTURE.404", "swamp Terrain Set"}, {"TEXTURE.405", "CORPSES golems"},
	{"TEXTURE.406", "CORPSES daylight cre"}, {"TEXTURE.407", "Swamp Ruins"}, {"TEXTURE.408", "Swamp Ruins snow"},
	{"TEXTURE.409", "Swamp Castle"}, {"TEXTURE.410", "Swamp Castle snow"}, {"TEXTURE.411", "Swamp Palace Int."},
	{"TEXTURE.412", "Swamp City A"}, {"TEXTURE.413", "Swamp City A snow"}, {"TEXTURE.414", "Swamp City B"},
	{"TEXTURE.415", "Swamp City B snow"}, {"TEXTURE.416", "Swamp City Int"}, {"TEXTURE.417", "Swamp City Walls"},
	{"TEXTURE.418", "Swamp City Walls"}, {"TEXTURE.419", "Swamp Crypt A"}, {"TEXTURE.420", "Swamp Crypt B"},
	{"TEXTURE.422", "Swamp Dungeons A"}, {"TEXTURE.423", "Swamp Dungeons B"}, {"TEXTURE.424", "Swamp Dungeons C"},
	{"TEXTURE.425", "Swamp Dungeons NEWCs"}, {"TEXTURE.426", "Swamp Farm"}, {"TEXTURE.427", "Swamp Farm snow"},
	{"TEXTURE.428", "Swamp Farm Interiors"}, {"TEXTURE.429", "Woodland Fences snow"}, {"TEXTURE.430", "Woodland Fences"},
	{"TEXTURE.431", "mentella crux chest"}, {"TEXTURE.432", "male artifacts"}, {"TEXTURE.433", "female artifacts"},
	{"TEXTURE.434", "Blood and Gore CHILD"}, {"TEXTURE.435", "Woodland Mages Guild"}, {"TEXTURE.437", "Woodland Mages Guild In"},
	{"TEXTURE.438", "Swamp Manor"}, {"TEXTURE.439", "Swamp Manor snow"}, {"TEXTURE.440", "Swamp Manor Interior"},
	{"TEXTURE.442", "Swamp Merchant Homes"}, {"TEXTURE.443", "Swamp Merchant Homes sn"}, {"TEXTURE.444", "Swamp Merchant Home Int"},
	{"TEXTURE.445", "Swamp Mines"}, {"TEXTURE.446", "Direni Test set"}, {"TEXTURE.447", "Swamp  Natural Caves"},
	{"TEXTURE.448", "Swamp Paintings"}, {"TEXTURE.449", "Generic Tapestries"}, {"TEXTURE.450", "Generic banners"},
	{"TEXTURE.451", "female nord"}, {"TEXTURE.452", "female nord"}, {"TEXTURE.453", "female breton"},
	{"TEXTURE.454", "female breton"}, {"TEXTURE.455", "female breton"}, {"TEXTURE.456", "female breton"},
	{"TEXTURE.457", "Bear"}, {"TEXTURE.458", "Swamp Tavern"}, {"TEXTURE.459", "Swamp Tavern Snow"},
	{"TEXTURE.460", "Swamp Tavern Int."}, {"TEXTURE.461", "Swamp Temple"}, {"TEXTURE.462", "Swamp Temple"},
	{"TEXTURE.463", "Swamp Temple Int."}, {"TEXTURE.464", "Swamp Village"}, {"TEXTURE.465", "Swamp Village snow"},
	{"TEXTURE.466", "Swamp Village Int."}, {"TEXTURE.467", "Totem holder"}, {"TEXTURE.468", "Swamp Sewer"},
	{"TEXTURE.469", "Swamp Roofs"}, {"TEXTURE.470", "Swamp Roofs"}, {"TEXTURE.473", "Lydandus"},
	{"TEXTURE.474", "Door Set"}, {"TEXTURE.475", "Fighter Mage"}, {"TEXTURE.476", "Fighter Mage"},
	{"TEXTURE.477", "Fighter Mage"}, {"TEXTURE.478", "Fighter Mage"}, {"TEXTURE.479", "Fighter Thief"},
	{"TEXTURE.480", "Heavy Thief"}, {"TEXTURE.481", "Light Fighter"}, {"TEXTURE.482", "Fighter Mage"},
	{"TEXTURE.483", "Light Thief"}, {"TEXTURE.484", "Light Thief"}, {"TEXTURE.485", "Normal Mage"},
	{"TEXTURE.486", "Normal Mage"}, {"TEXTURE.487", "Medium Fighter"}, {"TEXTURE.488", "Medium Fighter"},
	{"TEXTURE.489", "Thief Mage"}, {"TEXTURE.490", "Thief Mage"}, {"TEXTURE.491", "Female heavy horse"},
	{"TEXTURE.492", "Male heavy horse"}, {"TEXTURE.493", "Female medium horse"}, {"TEXTURE.494", "Male medium horse"},
	{"TEXTURE.495", "Female light horse"}, {"TEXTURE.500", "Rain Forest"}, {"TEXTURE.501", "Sub_Tropical"},
	{"TEXTURE.502", "Swamp"}, {"TEXTURE.503", "Desert"}, {"TEXTURE.504", "Temperate woodland"},
	{"TEXTURE.505", "Snow woodland"}, {"TEXTURE.506", "Woodland Hills"}, {"TEXTURE.507", "Snow woodland Hills"},
	{"TEXTURE.508", "Haunted woodland"}, {"TEXTURE.509", "Snow haunted woodlan"}, {"TEXTURE.510", "mountains"},
	{"TEXTURE.511", "Snow Mountains"},
};


// This array is missing: FMAP0I00.IMG; FMAP0I01.IMG; FMAP0I16.IMG
static char *IMGFiles[] = {
	{"AMAP00I0.IMG"}, {"AMAP01I0.IMG"}, {"BANK00I0.IMG"}, {"BANK01I0.IMG"}, {"BANK01I1.IMG"},
	{"BANK01I2.IMG"}, {"BIOG00I0.IMG"}, {"BOAR00I0.IMG"}, {"BODY00I0.IMG"}, {"BODY00I1.IMG"},
	{"BODY01I0.IMG"}, {"BODY01I1.IMG"}, {"BODY02I0.IMG"}, {"BODY02I1.IMG"}, {"BODY03I0.IMG"},
	{"BODY03I1.IMG"}, {"BODY04I0.IMG"}, {"BODY04I1.IMG"}, {"BODY05I0.IMG"}, {"BODY05I1.IMG"},
	{"BODY06I0.IMG"}, {"BODY06I1.IMG"}, {"BODY07I0.IMG"}, {"BODY07I1.IMG"}, {"BODY10I0.IMG"},
	{"BODY10I1.IMG"}, {"BODY11I0.IMG"}, {"BODY11I1.IMG"}, {"BODY12I0.IMG"}, {"BODY12I1.IMG"},
	{"BODY13I0.IMG"}, {"BODY13I1.IMG"}, {"BODY14I0.IMG"}, {"BODY14I1.IMG"}, {"BODY15I0.IMG"},
	{"BODY15I1.IMG"}, {"BODY16I0.IMG"}, {"BODY16I1.IMG"}, {"BODY17I0.IMG"}, {"BODY17I1.IMG"},
	{"BOOK00I0.IMG"}, {"BUTN00I0.IMG"}, {"BUTN01I0.IMG"}, {"BUTN02I0.IMG"}, {"BUTN03I0.IMG"},
	{"CHAR00I0.IMG"}, {"CHAR01I0.IMG"}, {"CHAR02I0.IMG"}, {"CHAR02I1.IMG"}, {"CHAR03I0.IMG"},
	{"CHAR03I1.IMG"}, {"CHAR04I0.IMG"}, {"CHAR05I0.IMG"}, {"CHAR05I1.IMG"}, {"CHGN00I0.IMG"},
	{"CMPA03I0.IMG"}, {"CNFG00I0.IMG"}, {"CNFG00I1.IMG"}, {"CNFG01I0.IMG"}, {"CNFG02I0.IMG"},
	{"CNFG03I0.IMG"}, {"CNFG04I0.IMG"}, {"CNFG06I0.IMG"}, {"COMPASS.IMG"}, {"COMPBOX.IMG"},
	{"CORT01I0.IMG"}, {"CUST00I0.IMG"}, {"CUST01I0.IMG"}, {"CUST02I0.IMG"}, {"CUST03I0.IMG"},
	{"CUST04I0.IMG"}, {"CUST05I0.IMG"}, {"CUST06I0.IMG"}, {"CUST07I0.IMG"}, {"CUST08I0.IMG"},
	{"CUST09I0.IMG"}, {"DAED00I0.IMG"}, {"DANK02I0.IMG"}, {"DESERT.IMG"}, {"DIE_00I0.IMG"},
	/*{"FMAP0I00.IMG"}, {"FMAP0I01.IMG"},*/ {"FMAP0I05.IMG"}, {"FMAP0I09.IMG"}, {"FMAP0I11.IMG"},
	/*{"FMAP0I16.IMG"},*/ {"FMAP0I17.IMG"}, {"FMAP0I18.IMG"}, {"FMAP0I19.IMG"}, {"FMAP0I20.IMG"},
	{"FMAP0I21.IMG"}, {"FMAP0I22.IMG"}, {"FMAP0I23.IMG"}, {"FMAP0I26.IMG"}, {"FMAP0I32.IMG"},
	{"FMAP0I33.IMG"}, {"FMAP0I34.IMG"}, {"FMAP0I35.IMG"}, {"FMAP0I36.IMG"}, {"FMAP0I37.IMG"},
	{"FMAP0I38.IMG"}, {"FMAP0I39.IMG"}, {"FMAP0I40.IMG"}, {"FMAP0I41.IMG"}, {"FMAP0I42.IMG"},
	{"FMAP0I43.IMG"}, {"FMAP0I44.IMG"}, {"FMAP0I45.IMG"}, {"FMAP0I46.IMG"}, {"FMAP0I47.IMG"},
	{"FMAP0I48.IMG"}, {"FMAP0I49.IMG"}, {"FMAP0I50.IMG"}, {"FMAP0I51.IMG"}, {"FMAP0I52.IMG"},
	{"FMAP0I53.IMG"}, {"FMAP0I54.IMG"}, {"FMAP0I55.IMG"}, {"FMAP0I56.IMG"}, {"FMAP0I57.IMG"},
	{"FMAP0I58.IMG"}, {"FMAP0I59.IMG"}, {"FMAP0I60.IMG"}, {"FMAP0I61.IMG"}, {"FMAPAI00.IMG"},
	{"FMAPAI01.IMG"}, {"FMAPAI16.IMG"}, {"FMAPBI00.IMG"}, {"FMAPBI01.IMG"}, {"FMAPBI16.IMG"},
	{"FMAPCI01.IMG"}, {"FMAPCI16.IMG"}, {"FMAPDI01.IMG"}, {"FMAPDI16.IMG"}, {"FRAM00I0.IMG"},
	{"GILD00I0.IMG"}, {"GILD01I0.IMG"}, {"GNRC00I0.IMG"}, {"GNRC01I0.IMG"}, {"GRAD00I0.IMG"},
	{"GRAD01I0.IMG"}, {"HERB00I0.IMG"}, {"ICON00I0.IMG"}, {"INFO00I0.IMG"}, {"INFO01I0.IMG"},
	{"INVE00I0.IMG"}, {"INVE01I0.IMG"}, {"INVE02I0.IMG"}, {"INVE03I0.IMG"}, {"INVE04I0.IMG"},
	{"INVE05I0.IMG"}, {"INVE06I0.IMG"}, {"INVE07I0.IMG"}, {"INVE08I0.IMG"}, {"INVE09I0.IMG"},
	{"INVE0FI0.IMG"}, {"INVE10I0.IMG"}, {"INVE11I0.IMG"}, {"INVE12I0.IMG"}, {"INVE13I0.IMG"},
	{"INVE14I0.IMG"}, {"INVE15I0.IMG"}, {"ITEM00I0.IMG"}, {"ITEM01I0.IMG"}, {"LAMP00I0.IMG"},
	{"LGBK00I0.IMG"}, {"LOAD00I0.IMG"}, {"MAGE00I0.IMG"}, {"MAIN00I0.IMG"}, {"MAIN01I0.IMG"},
	{"MAIN02I0.IMG"}, {"MAIN03I0.IMG"}, {"MAIN04I0.IMG"}, {"MAIN05I0.IMG"}, {"MAP100I0.IMG"},
	{"MASK00I0.IMG"}, {"MASK01I0.IMG"}, {"MASK02I0.IMG"}, {"MASK04I0.IMG"}, {"MASK05I0.IMG"},
	{"MASK06I0.IMG"}, {"MBRD00I0.IMG"}, {"MENU.IMG"}, {"MOVE00I0.IMG"}, {"MOVE01I0.IMG"},
	{"MRED00I0.IMG"}, {"MRED01I0.IMG"}, {"NITE00I0.IMG"}, {"NITE01I0.IMG"}, {"NITE02I0.IMG"},
	{"NITE03I0.IMG"}, {"OPTN00I0.IMG"}, {"PICK00I0.IMG"}, {"PICK01I0.IMG"}, {"PICK02I0.IMG"},
	{"PICK03I0.IMG"}, {"POPU00I0.IMG"}, {"PRIS00I0.IMG"}, {"REPR01I0.IMG"}, {"REPR02I0.IMG"},
	{"REST00I0.IMG"}, {"REST01I0.IMG"}, {"REST02I0.IMG"}, {"SAVE00I0.IMG"}, {"SCBG00I0.IMG"},
	{"SCBG01I0.IMG"}, {"SCBG02I0.IMG"}, {"SCBG03I0.IMG"}, {"SCBG04I0.IMG"}, {"SCBG05I0.IMG"},
	{"SCBG06I0.IMG"}, {"SCBG07I0.IMG"}, {"SCBG08I0.IMG"}, {"SCRL04I0.IMG"}, {"SCRL05I0.IMG"},
	{"SCRL07I0.IMG"}, {"SCRL08I0.IMG"}, {"SCRL09I0.IMG"}, {"SCRL10I0.IMG"}, {"SCRL11I0.IMG"},
	{"SCRL12I0.IMG"}, {"SCRL13I0.IMG"}, {"SCRL14I0.IMG"}, {"SHOP00I0.IMG"}, {"SHOP01I0.IMG"},
	{"SHOP01I1.IMG"}, {"SHOP02I0.IMG"}, {"SHOP02I1.IMG"}, {"SHOP03I0.IMG"}, {"SHOP03I1.IMG"},
	{"SHOP04I0.IMG"}, {"SHOP04I1.IMG"}, {"SPBK00I0.IMG"}, {"SPBK01I0.IMG"}, {"SUN_00I0.IMG"},
	{"TALK00I0.IMG"}, {"TALK01I0.IMG"}, {"TALK02I0.IMG"}, {"TALK03I0.IMG"}, {"TAMRIEL.IMG"},
	{"TAMRIEL2.IMG"}, {"TELE00I0.IMG"}, {"TITL00I0.IMG"}, {"TMAP00I0.IMG"}, {"TMPL00I0.IMG"},
	{"TMPL01I0.IMG"}, {"TOWN00I0.IMG"}, {"TRAV00I0.IMG"}, {"TRAV01I0.IMG"}, {"TRAV01I1.IMG"},
	{"TRAV02I0.IMG"}, {"TRAV0I00.IMG"}, {"TRAV0I01.IMG"}, {"TRAV0I03.IMG"}, {"TRAV0I04.IMG"},
	{"TRAVAI05.IMG"}, {"TRAVBI05.IMG"}, {"TRAVCI05.IMG"}, {"TRAVDI05.IMG"}, {"TVRN00I0.IMG"},
	{"WERE00I0.IMG"}, {"WERE01I0.IMG"}, {"WOLF00I0.IMG"},
};


static char *CIFFiles[] = {
	{"APAINT.CIF"}, {"BPAINT.CIF"}, {"CNFG05I0.CIF"}, {"CPAINT.CIF"}, {"DPAINT.CIF"},
	{"EPAINT.CIF"}, {"FACE00I0.CIF"}, {"FACE01I0.CIF"}, {"FACE02I0.CIF"}, {"FACE03I0.CIF"},
	{"FACE04I0.CIF"}, {"FACE05I0.CIF"}, {"FACE06I0.CIF"}, {"FACE07I0.CIF"}, {"FACE10I0.CIF"},
	{"FACE11I0.CIF"}, {"FACE12I0.CIF"}, {"FACE13I0.CIF"}, {"FACE14I0.CIF"}, {"FACE15I0.CIF"},
	{"FACE16I0.CIF"}, {"FACE17I0.CIF"}, {"FACES.CIF"}, {"FIRE00C6.CIF"}, {"FPAINT.CIF"},
	{"FRST00C6.CIF"}, {"GPAINT.CIF"}, {"HPAINT.CIF"}, {"INVE16I0.CIF"}, {"IPAINT.CIF"},
	{"JPAINT.CIF"}, {"KIDS00I0.CIF"}, {"KPAINT.CIF"}, {"LPAINT.CIF"}, {"MCOL00I0.CIF"},
	{"MJIC00C6.CIF"}, {"MPAINT.CIF"}, {"NPAINT.CIF"}, {"OPAINT.CIF"}, {"PNTER.CIF"},
	{"POIS00C6.CIF"}, {"PPAINT.CIF"}, {"QPAINT.CIF"}, {"RPAINT.CIF"}, {"SHOK00C6.CIF"},
	{"SPAINT.CIF"}, {"TPAINT.CIF"}, {"UPAINT.CIF"}, {"VAMP00I0.CIF"}, {"VPAINT.CIF"},
	{"WEAPO101.CIF"}, {"WEAPO102.CIF"}, {"WEAPO104.CIF"}, {"WEAPO105.CIF"}, {"WEAPO106.CIF"},
	{"WEAPO107.CIF"}, {"WEAPO108.CIF"}, {"WEAPON00.CIF"}, {"WEAPON01.CIF"}, {"WEAPON02.CIF"},
	{"WEAPON03.CIF"}, {"WEAPON04.CIF"}, {"WEAPON05.CIF"}, {"WEAPON06.CIF"}, {"WEAPON07.CIF"},
	{"WEAPON08.CIF"}, {"WEAPON09.CIF"}, {"WEAPON10.CIF"}, {"WEAPON11.CIF"}, {"WPAINT.CIF"},
};


static char *RCIFiles[] = {
	{"BUTTONS.RCI"}, {"CHLD00I0.RCI"}, {"MPOP.RCI"}, {"NOTE.RCI"}, {"SPOP.RCI"}, {"TFAC00I0.RCI"},
};


static char *LocaleNames[] = {
	{"Alik'r Desert"}, {"Dragontail Mountains"}, {"Glenpoint Foothills"}, {"Daggerfall Bluffs"},
	{"Yeorth Burrowland"}, {"Dwynnen"}, {"Ravennian Forest"}, {"Devilrock"},
	{"Malekna Forest"}, {"Isle of Balfiera"}, {"Bantha"}, {"Dak'fron"},
	{"Islands in the Western Iliac Bay"}, {"Tamarilyn Point"}, {"Lainlyn Cliffs"}, {"Bjoulsae River"},
	{"Wrothgarian Mountains"}, {"Daggerfall"}, {"Glenpoint"}, {"Betony"},
	{"Sentinel"}, {"Anticlere"}, {"Lainlyn"}, {"Wayrest"},
	{"Gen Tem High Rock village"}, {"Gen Rai Hammerfell village"}, {"Orsinium Area"}, {"Skeffington Wood"},
	{"Hammerfell bay coast"}, {"Hammerfell sea coast"}, {"High Rock bay coast"}, {"High Rock sea coast"},
	{"Northmoor"}, {"Menevia"}, {"Alcaire"}, {"Koegria"},
	{"Bhoriane"}, {"Kambria"}, {"Phrygias"}, {"Urvaius"},
	{"Ykalon"}, {"Daenia"}, {"Shalgora"}, {"Abibon-Gora"},
	{"Kairou"}, {"Pothago"}, {"Myrkwasa"}, {"Ayasofya"},
	{"Tigonus"}, {"Kozanset"}, {"Satakalaam"}, {"Totambu"},
	{"Mournoth"}, {"Ephesus"}, {"Santaki"}, {"Antiphyllos"},
	{"Bergama"}, {"Gavaudon"}, {"Tulune"}, {"Glenumbra Moors"},
	{"Ilessan Hills"}, {"Cybiades"}
};


#endif