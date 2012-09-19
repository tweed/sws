#include "stdafx.h"
#include "../reaper/localize.h"

#include "RprTake.hxx"
#include "RprItem.hxx"
#include "RprTrack.hxx"
#include "RprException.hxx"


RprTake::RprTake(MediaItem_Take *take)
{
    if(take == NULL)
        throw RprLibException(__LOCALIZE("Media item take is NULL","sws_mbox"), false);
    mTake = take;
}

MediaItem_Take* RprTake::toReaper() const
{
    return mTake;
}

double RprTake::getPlayRate() const
{
    double playRate = *(double *)GetSetMediaItemTakeInfo(mTake, "D_PLAYRATE", NULL);
    return playRate;
}

void RprTake::setPlayRate(double playRate)
{
    GetSetMediaItemTakeInfo(mTake, "D_PLAYRATE", (void *)&playRate);
}

RprItem RprTake::getParent() const
{
    RprItem item((MediaItem *)GetSetMediaItemTakeInfo(mTake, "P_ITEM", NULL));
    return item;
}

PCM_source *RprTake::getSource()
{
    return (PCM_source *)GetSetMediaItemTakeInfo(mTake, "P_SOURCE", NULL);
}

double RprTake::getStartOffset() const
{
    return *(double *)GetSetMediaItemTakeInfo(mTake, "D_STARTOFFS", NULL);
}

void RprTake::setStartOffset(double offset)
{
    GetSetMediaItemTakeInfo(mTake, "D_STARTOFFS", &offset);
}

GUID *RprTake::getGUID() const
{
    return (GUID *)GetSetMediaItemTakeInfo(mTake, "GUID", NULL);
}

bool RprTake::isFile()
{
    PCM_source *source = getSource();
    std::string fileName(source->GetFileName());
    return !fileName.empty();
}

bool RprTake::isMIDI()
{
    PCM_source *source = getSource();
    std::string takeType(source->GetType());
    return takeType == "MIDI" || takeType == "MIDIPOOL";
}

void RprTake::setName(const char *name)
{
    GetSetMediaItemTakeInfo(mTake, "P_NAME", (void *)name);
}

PCM_source *RprTake::createSource(const char *fileName)
{
    return PCM_Source_CreateFromFile(fileName);
}

void RprTake::setSource(PCM_source *source, bool keepOld)
{
    PCM_source *oldSource = getSource();
    GetSetMediaItemTakeInfo(mTake, "P_SOURCE", (void *)source);
    if(!keepOld)
        delete oldSource;
}

void RprTake::openEditor()
{
    void *midiEditor = MIDIEditor_GetActive();
    if(isMIDI() && midiEditor) {
        getSource()->Extended(PCM_SOURCE_EXT_OPENEDITOR, midiEditor, (void *)getParent().getTrack().getTrackIndex(), (void *)getName());
    }
}

const char *RprTake::getName()
{
    return (const char *)GetSetMediaItemTakeInfo(mTake, "P_NAME", NULL);
}

RprTake RprTake::createFromMidiEditor()
{
    void *midiEditor = MIDIEditor_GetActive();
    RprTake take(MIDIEditor_GetTake(midiEditor));
    return take;
}
