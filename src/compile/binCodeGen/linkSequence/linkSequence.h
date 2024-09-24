#pragma once
#include "../../../data_structures/linkedList/linkedList.h"
#include "../../instruction/instruction.h"
#include <stdlib.h>

typedef struct {
    List links;
    ListIter linkToClose;
    ListIter itForward;
} _LinkSequence, *LinkSequence;

LinkSequence createLinkSequence ();
void deleteLinkSequence (LinkSequence sequence);
void addBacklinkSequence (LinkSequence sequence, word addrFrom);
word getCurrentAddrFromSequence (LinkSequence sequence);
void closeBacklinkSequence (LinkSequence sequence, word addrTo);
void closeFirstBackinkSequence (LinkSequence sequence, word addrTo);
void setBeginSequence (LinkSequence sequence);
bool isEndSequence (LinkSequence sequence);
word nextAddrToSequence (LinkSequence sequence);