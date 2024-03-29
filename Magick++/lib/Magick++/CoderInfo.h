// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001, 2002
//
// CoderInfo Definition
//
// Container for image format support information.
//

#if !defined (Magick_CoderInfo_header)
#define Magick_CoderInfo_header  1

#include "Magick++/Include.h"
#include <string>

namespace Magick
{
  class MagickPPExport CoderInfo
  {
  public:

    enum MatchType {
      AnyMatch,  // match any coder
      TrueMatch, // match coder if true
      FalseMatch // match coder if false
    };

    // Default constructor
    CoderInfo ( void );

    // Copy constructor
    CoderInfo ( const CoderInfo &coder_ );

    // Construct with coder name
    CoderInfo ( const std::string &name_ );

    // Destructor
    ~CoderInfo ( void );

    // Format description
    std::string description( void ) const;

    // Format supports multiple frames
    bool isMultiFrame( void ) const;

    // Format is readable
    bool isReadable( void ) const;

    // Format is writeable
    bool isWritable( void ) const;

    // Format mime type
    std::string mimeType( void ) const;

    // Format name
    std::string name( void ) const;

    // Assignment operator
    CoderInfo& operator= ( const CoderInfo &coder_ );

    //
    // Implemementation methods
    //
    CoderInfo ( const MagickCore::MagickInfo *magickInfo_ );

  private:

    std::string _description;
    bool        _isReadable;
    bool        _isWritable;
    bool        _isMultiFrame;
    std::string _mimeType;
    std::string _name;

  };
} // namespace Magick

//
// Inlines
//


#endif // Magick_CoderInfo_header
