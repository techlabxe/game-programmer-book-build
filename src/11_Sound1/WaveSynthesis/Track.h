#ifndef INCLUDED_TRACK_H
#define INCLUDED_TRACK_H

struct Note;

class Track{
public:
	Track();
	~Track();
	void setData( const Note* notes, int noteNumber );
	void addWave( short* buffer, int bufferSize, int frequency ) const;
private:
	void addNote( 
		short* buffer, 
		int waveWidth, 
		int name, 
		int sampleNumber,
		int frequency ) const;

	Note* mNotes;
	int mNoteNumber;
};

#endif
