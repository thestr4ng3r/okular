// fontpool.h
//
// (C) 2001 Stefan Kebekus
// Distributed under the GPL

#ifndef _FONTPOOL_H
#define _FONTPOOL_H

#include <qlist.h>
#include <qstringlist.h>
#include <qobject.h>

#include "font.h"

class KProcess;
class KShellProcess;

#define NumberOfMFModes 3
#define DefaultMFMode 1

extern const char *MFModes[];
extern const char *MFModenames[];
extern const int   MFResolutions[];


/**
 *  A list of fonts and a compilation of utility functions
 *
 * This class holds a list of fonts and is able to perform a number of
 * functions on each of the fonts. The main use of this class is that
 * it is able to control a concurrently running "kpsewhich" programm
 * which is used to locate and load the fonts.
 *
 * @author Stefan Kebekus   <kebekus@kde.org>
 *
 *
 **/

class fontPool : public QObject {
Q_OBJECT

 public:
  /** Default constructor. */ 
  fontPool( void );

  /** Default destructor. */ 
  ~fontPool( void );

  /** Method used to set the MetafontMode for the PK font files. This
      data is used when loading fonts. Currently, a change here will
      be applied only to those font which were not yet loaded
      ---expect funny results when changing the data in the
      mid-work. The integer argument must be smaller than
      NumberOfMFModes, which is defined in fontpool.h and refers to a
      Mode/Resolutin pair in the lists MFModes and MFResolutins which
      are also defined in fontpool.h and implemented in
      fontpool.cpp. Returns the mode number of the mode which was
      actually set ---if an invalid argument is given, this will be
      the DefaultMFMode as defined in fontPool.h */
  unsigned int setMetafontMode( unsigned int );

  /** Return the currently set MetafontMode */

  unsigned int getMetafontMode(void) {return MetafontMode;};

  /** Says whether fonts will be generated by running MetaFont, or a
      similar programm. If (flag == 0), fonts will not be generated,
      otherwise they will. */
  void setMakePK( int flag );
  
  /** This method adds a font to the list. If the font is not
      currently loaded, it's file will be located and font::load_font
      will be called. Since this is done using a concurrently running
      process, there is no guarantee that the loading is already
      performed when the method returns.  */
  class font *appendx(char *fontname, long checksum, int scale, int design, class font *vfparent);

  /** Prints very basic debugging information about the fonts in the
      pool to the kdDebug output stream. */
  QString status(); 

  /** Releases all the shrunken bitmaps in all fonts. */
  void reset_fonts(void);

  /** Checks if all the fonts are loaded and returns 0 if that is
      so. Otherwise, returns -1. The argument is used only internally
      and should always be 0, except for internal use. Actually, the
      argument works in the following manner: if pass==0, then the
      method looks for pk-fonts and virtual fonts, and disables
      automatic pk generation, even if it is turned on with the
      setMakePK-method. If pass!=0, then only pk fonts are looked for,
      and fonts are generated, if necessary. This method works in
      stages: first, it is called with pass=0, and vf-fonts are
      loaded. These may define further pk-fonts. The pk-fonts will be
      loaded/generated in the second stage with pass!=0 which is
      called automatically when the first instance of the kpsewhich
      programm exists.  Calling this method will ALWAYS emit the
      signal fonts_info. */
  char check_if_fonts_are_loaded(unsigned char pass=0);

  /** This is the list which actually enumerates the fonts in the
      fontPool */
  QList<class font> fontList; 

  /** This method marks all fonts in the fontpool as "not in use". The
      fonts are, however, not removed from memory until the method
      release_fonts is called. The method is called when the dvi-file
      is closed. Because the next dvi-file which will be loaded is
      likely to use most of the fonts again, this method implements a
      convenient way of re-using fonts without loading them
      repeatedly. */
  void mark_fonts_as_unused(void);

  /** This methods removes all fonts from the fontpool (and thus from
      memory) which are labeled "not in use". For explanation, see the
      mark_fonts_as_unused method. */
  void release_fonts(void);

signals:
  /** Emitted to indicate that the progress dialog should show up now. */
  void show_progress(void);

  /** Emitted to indicate that all the fonts have now been loaded so
      that the first page can be rendered. */
  void fonts_have_been_loaded(void);

  /** The title says it all... */
  void hide_progress_dialog(void);

  /** Emitted at the start of a kpsewhich job to indicate how many
      fonts there are to be looked up/generated. */
  void totalFontsInJob(int);

  /** Emitted during the kpsewhich job to give the total number of
    fonts which have been looked up/generated. */
  void numFoundFonts(int);

  /** Emitted to pass output of MetaFont and friends on to the user
      interface. */
  void MFOutput(QString);

  /** Emitted when a kpsewhich run is started in order to clear the
      info dialog window. */
  void new_kpsewhich_run(QString);

  /** Emitted when the font-pool has changed. The class receiving the
      signal might whish to call status() in order to receive the
      data. We don't send the data here directly as the compilation of
      the string is quite costy. */
  void fonts_info(class fontPool *);

public slots:
  /** Aborts the font generation. Nasty. */
  void abortGeneration(void);

  /** For internal purposes only. This slot is called when the
      kpsewhich program has terminated. */
  void kpsewhich_terminated(KProcess *);

/** For internal purposess only. This slot is called when MetaFont is
    run via the kpsewhich programm. The MetaFont output is transmitted
    to the fontpool via this slot. */
 void mf_output_receiver(KProcess *, char *, int);

/** For internal purposess only. This slot is called when kpsewhich
    outputs the name of a font which has been found or was
    generated. */
 void kpsewhich_output_receiver(KProcess *, char *, int);

private:
 /** This flag determined whether we try to have MetaFont generate the
     font if a bitmap is not available. If makepk == 0, then bitmaps
     are NOT generated. */
 int            makepk;

 /** This integer determines the Metafont-Mode which is used in
     generating bitmaps. The value must satisfy 0 <= MetafontMode <
     NumberOfMFModes, and refers to an entry in the lists
     MFModenames/MFModes and MFResolutions which are defined in
     fontpool.cpp */
 unsigned int   MetafontMode;

 /** This QString is used to collect the output of the MetaFont
     programm while we are waiting for a full screen line */
 QString        MetafontOutput;

 QString        kpsewhichOutput;

  /** This is the ShellProcess which is used to run the kpsewhich
      command which locates the font-files for us. */
 KShellProcess *proc;

 /** FontProgress; the progress dialog used when generating fonts. */
 class fontProgressDialog *progress;
};

#endif //ifndef _FONTPOOL_H
