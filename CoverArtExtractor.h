
//	apt install libtag1-dev

// `taglib-config --cflags` `taglib-config --libs`
#include <stdio.h>
#include <string.h>

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/tstring.h>

#include <taglib/apetag.h>
#include <taglib/asftag.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/mp4tag.h>
#include <taglib/xiphcomment.h>
#include <taglib/modtag.h>
#include <taglib/infotag.h>
#include <taglib/apefile.h>
#include <taglib/asffile.h>
#include <taglib/flacfile.h>
#include <taglib/modfilebase.h>
#include <taglib/mp4file.h>
#include <taglib/mpcfile.h>
#include <taglib/mpegfile.h>
#include <taglib/oggfile.h>
#include <taglib/rifffile.h>
#include <taglib/trueaudiofile.h>
#include <taglib/wavpackfile.h>

std::vector<unsigned char>	ExtractCoverArt( const char * pszFilePath )
{
	printf( "----------------------------------------\n");
	printf( "file    : %s\n", pszFilePath );

	TagLib::FileRef	file( pszFilePath );
	TagLib::Tag*	piTag	= file.tag();

	if( NULL != piTag )
	{
		TagLib::File*				piFile		= file.file();
		TagLib::APE::File*			piFileAPE	= dynamic_cast<TagLib::APE::File*>(piFile);
		TagLib::ASF::File *			piFileASF	= dynamic_cast<TagLib::ASF::File*>(piFile);
		TagLib::FLAC::File *		piFileFLAC	= dynamic_cast<TagLib::FLAC::File*>(piFile);
		TagLib::Mod::FileBase  *	piFileMod	= dynamic_cast<TagLib::Mod::FileBase*>(piFile);
		TagLib::MP4::File *			piFileMP4	= dynamic_cast<TagLib::MP4::File*>(piFile);
		TagLib::MPC::File *			piFileMPC	= dynamic_cast<TagLib::MPC::File*>(piFile);
		TagLib::MPEG::File  *		piFileMPEG	= dynamic_cast<TagLib::MPEG::File*>(piFile);
		TagLib::Ogg::File *			piFileOgg	= dynamic_cast<TagLib::Ogg::File*>(piFile);
		TagLib::RIFF::File *		piFileRIFF	= dynamic_cast<TagLib::RIFF::File*>(piFile);
		TagLib::TrueAudio::File *	piFileTA	= dynamic_cast<TagLib::TrueAudio::File*>(piFile);
		TagLib::WavPack::File *		piFileWav	= dynamic_cast<TagLib::WavPack::File*>(piFile);
		
		TagLib::ID3v2::Tag*			piTagID3v2	= NULL;

		printf(
			"FileType: "
			"APE:%p, "	"ASP:%p, "	"FLAC:%p, "	"Mod:%p, "	"MP4:%p, "	"MPC:%p, "	"MPEG:%p, "	"Ogg:%p, "	"RIFF:%p, "	"TrueAudio:%p, "	"WavPack:%p, "	"\n",
			piFileAPE,	piFileASF,	piFileFLAC,	piFileMod,	piFileMP4,	piFileMPC,	piFileMPEG,	piFileOgg,	piFileRIFF,	piFileTA,			piFileWav );

		printf( "title   : %s\n", piTag->title().toCString(true) );
		printf( "artist  : %s\n", piTag->artist().toCString(true) );
		printf( "album   : %s\n", piTag->album().toCString(true) );
		printf( "year    : %d\n", piTag->year());
		printf( "comment : %s\n", piTag->comment().toCString(true) );

		if( NULL != piFileFLAC )
		{
			if( 0 < piFileFLAC->pictureList().size() )
			{
				TagLib::FLAC::Picture* pic = piFileFLAC->pictureList()[0];

			    std::vector<unsigned char>	data( pic->data().size());
			    memcpy( data.data(), pic->data().data(), data.size() );

				printf("CoverArt: %zu [bytes] FLAC\n", data.size() );
				return	data;
			}

			piTagID3v2	= piFileFLAC->ID3v2Tag(false);
		}
		else if( NULL != piFileMP4 )
		{
			TagLib::MP4::Tag* tag	= piFileMP4->tag();
			TagLib::MP4::ItemListMap itemsListMap = tag->itemListMap();
			TagLib::MP4::Item coverItem = itemsListMap["covr"];
			TagLib::MP4::CoverArtList coverArtList = coverItem.toCoverArtList();

			if (!coverArtList.isEmpty())
			{
			    TagLib::MP4::CoverArt	coverArt = coverArtList.front();

			    std::vector<unsigned char>	data( coverArt.data().size());
			    memcpy( data.data(), coverArt.data().data(), data.size() );

				printf("CoverArt: %zu [bytes] MP4\n", data.size() );
				return	data;
			}
		}
		else if( NULL != piFileASF )
		{
			const TagLib::ASF::AttributeListMap&	attrListMap = piFileASF->tag()->attributeListMap();
			const TagLib::ASF::AttributeList&		attrList	= attrListMap["WM/Picture"];
			TagLib::ASF::Picture					pic			= attrList[0].toPicture();

		    std::vector<unsigned char>	data( pic.picture().size());
		    memcpy( data.data(), pic.picture().data(), data.size() );

			printf("CoverArt: %zu [bytes] ASF\n", data.size() );
			return	data;
		}
		else if( NULL != piFileMPEG )
		{
			piTagID3v2	= piFileMPEG->ID3v2Tag(false);
		}

		if( NULL != piTagID3v2 )
		{
			TagLib::ID3v2::FrameList	frameList = piTagID3v2->frameList("APIC");
			if( !frameList.isEmpty() )
			{
				TagLib::ID3v2::AttachedPictureFrame*	coverImg = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());

			    std::vector<unsigned char>	data( coverImg->picture().size());
			    memcpy( data.data(), coverImg->picture().data(), data.size() );

				printf("CoverArt: %zu [bytes] ID3v2\n", data.size() );
				return	data;
			}
		}
	}

	return	std::vector<unsigned char>();
}
