#include "linkbutton.h"

LinkButton::LinkButton(int i,QString filePath)
{
    i1=i;
    filePath1=filePath;
    connect(this, SIGNAL(clicked()), this, SLOT(sendPointer()));
}

LinkButton::~LinkButton()
{
}

void LinkButton::sendPointer()
{
    emit getPointer(i1, filePath1);
}
