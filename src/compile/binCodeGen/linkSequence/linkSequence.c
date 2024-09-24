#include "linkSequence.h"

typedef struct {
    bool isFilled;
    word addrTo;
    word addrFrom;
} _BACKLINK, *BACKLINK;

BACKLINK createBackLink ();
void deleteBackLink (BACKLINK backLink);
bool isFilledBackLink (BACKLINK backLink);
word getAddrToBackLink (BACKLINK backLink);
void setAddrToBackLink (BACKLINK backLink, word addr);
word getAddrFromBackLink (BACKLINK backLink);
void setAddrFromBackLink (BACKLINK backLink, word addr);

void _setUnfilledLink (LinkSequence sequence);

BACKLINK createBackLink () {
    BACKLINK backLink = (BACKLINK)malloc(sizeof(_BACKLINK));
    backLink->isFilled = false;
    return backLink;
}

void deleteBackLink (BACKLINK backLink) {
    free(backLink);
}

bool isFilledBackLink (BACKLINK backLink) {
    return backLink->isFilled;
}

word getAddrToBackLink (BACKLINK backLink) {
    return backLink->addrTo;
}

void setAddrToBackLink (BACKLINK backLink, word addr) {
    backLink->addrTo = addr;
    backLink->isFilled = true;
}

word getAddrFromBackLink (BACKLINK backLink) {
    return backLink->addrFrom;    
}

void setAddrFromBackLink (BACKLINK backLink, word addr) {
    backLink->addrFrom = addr;            
}

LinkSequence createLinkSequence () {
    LinkSequence res = (LinkSequence)malloc(sizeof(_LinkSequence));
    res->links = createList();
    return res;
}

void deleteLinkSequence (LinkSequence sequence) {
    for (ListIter it = beginList(sequence->links); iterNotEquals(it, endList(sequence->links)); it = iterNext(it)) {
        deleteBackLink(iterFetch(BACKLINK, it));
    }

    deleteList(sequence->links);
    free(sequence);
}

void addBacklinkSequence (LinkSequence sequence, word addrFrom) {
    BACKLINK link = createBackLink();
    setAddrFromBackLink(link, addrFrom);

    insertToList(sequence->links, iterPrev(endList(sequence->links)), link);
    sequence->linkToClose = iterPrev(endList(sequence->links));
}

word getCurrentAddrFromSequence (LinkSequence sequence) {
    return getAddrFromBackLink(iterFetch(BACKLINK, sequence->linkToClose));
}

void closeBacklinkSequence (LinkSequence sequence, word addrTo) {
    setAddrToBackLink(iterFetch(BACKLINK, sequence->linkToClose), addrTo);
    _setUnfilledLink(sequence);
}

void closeFirstBackinkSequence (LinkSequence sequence, word addrTo) {
    setAddrToBackLink(iterFetch(BACKLINK, beginList(sequence->links)), addrTo);
}

void setBeginSequence (LinkSequence sequence) {
    sequence->itForward = beginList(sequence->links);
}

bool isEndSequence (LinkSequence sequence) {
    return iterEquals(sequence->itForward, endList(sequence->links));
}

word nextAddrToSequence (LinkSequence sequence) {
    word currentAddr = getAddrToBackLink(iterFetch(BACKLINK, sequence->itForward));
    sequence->itForward = iterNext(sequence->itForward);
    return currentAddr;
}

void _setUnfilledLink (LinkSequence sequence) {
    // пятимся назад в поисках не закрытой ссылки
    while (iterNotEquals(sequence->linkToClose, beginList(sequence->links)) &&
        isFilledBackLink(iterFetch(BACKLINK, sequence->linkToClose))) {
        
        sequence->linkToClose = iterPrev(sequence->linkToClose);
    }
}