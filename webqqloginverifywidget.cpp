
#include "webqqaccount.h"

#include <QtGui/QtGui>  

#include "webqqloginverifywidget.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#define EFFECTIVELEN 10
#define GETBYTELEN   2

/*---JPEG/JPG(1)---*/
#define IMAGE_JPEG_JPG "JPEG/JPG"
#define JPEG_FIRSTBIT  "FF"
#define JPEG_SECONDBIT "D8"

/*---BMP(2)---*/
#define IMAGE_BMP     "BMP"
#define BMP_FIRSTBIT  "42"
#define BMP_SECONDBIT "4D"

/*---PNG(3)---*/
#define IMAGE_PNG     "PNG"
#define PNG_FIRSTBIT  "89"
#define PNG_SECONDBIT "50"

/*---GIF(4)---*/
#define IMAGE_GIF     "GIF"
#define GIF_FIRSTBIT  "47"
#define GIF_SECONDBIT "49"
#define GIF_THIRDBIT  "46"
#define GIF_FIFTHBIT1 "39"
#define GIF_FIFTHBIT2 "37"

/*---TIFF(5)---*/
#define IMAGE_TIFF      "TIFF"
#define TIFF_FIRSTBIT   "4D"
#define TIFF_SECONDBIT  "4D"
#define TIFF_FIRSTBIT2  "49"
#define TIFF_SECONDBIT2 "49"

/*---ICO(6)---*/
#define IMAGE_ICO    "ICO"
#define ICO_THIRDBIT "1"  // 01
#define ICO_FIFTHBIT "1"  // 01

/*---TGA(7)---*/
#define IMAGE_TGA    "TGA"
#define TGA_THIRDBIT "2"  // 02
#define TGA_FIFTHBIT "0"  // 00

/*---CUR(8)---*/
#define IMAGE_CUR    "CUR"
#define CUR_THIRDBIT "2"  // 02
#define CUR_FIFTHBIT "1"  // 01

/*---PCX(9)---*/
#define IMAGE_PCX    "PCX"
#define PCX_FIRSTBIT "A"  // 0A

/*---IFF(10)---*/
#define IMAGE_IFF     "IFF"
#define IFF_FIRSTBIT  "46"
#define IFF_SECONDBIT "4F"
#define IFF_THIRDBIT  "52"
#define IFF_FORTHBIT  "4D"

/*---ANI(11)---*/
#define IMAGE_ANI     "ANI"
#define ANI_FIRSTBIT  "52"
#define ANI_SECONDBIT "49"
#define ANI_THIRDBIT  "46"
#define ANI_FORTHBIT  "46"

char MImagesType[12][10] = {"JPEG","JPG","BMP","PNG","GIF","TIFF","aICO","TGA",
                              "CUR","PCX","IFF","ANI"};
char LImagesType[12][10] = {"jpeg","jpg","bmp","png","gif","tiff","aico","tga",
                              "cur","pcx","iff","ani"};

void reserve(char *p, char *q)
{
     while(p < q){
          *p ^= *q;
          *q ^= *p;
          *p ^= *q;
          p++;
          q--;
     }
}

char *JudgmentImageByDate(char *Filename)
{
     FILE *fd;
     unsigned char *imagebuf;
     int filesize = 0;
     unsigned char firstBit[3];
     unsigned char secondBit[3];
     unsigned char thirdBit[3];
     unsigned char forthBit[3];
     unsigned char fifthBit[3];

     assert(Filename != NULL);

     if( (fd = fopen(Filename, "rb")) == NULL){
          perror("fopen error\n");
          exit(1);
     }

     fseek(fd, 0, SEEK_END);
     filesize = ftell(fd);
     rewind(fd);

     imagebuf = (unsigned char *)malloc(EFFECTIVELEN * sizeof(unsigned char) + 1);
     fread(imagebuf, sizeof(unsigned char), EFFECTIVELEN, fd);

     snprintf(firstBit, sizeof(firstBit), "%X", imagebuf[0]);
     snprintf(secondBit, sizeof(secondBit), "%X", imagebuf[1]);
     snprintf(thirdBit, sizeof(thirdBit), "%X", imagebuf[2]);
     snprintf(forthBit, sizeof(forthBit), "%X", imagebuf[3]);
     snprintf(fifthBit, sizeof(fifthBit), "%X", imagebuf[4]);

     if( (0 == strncmp(firstBit, JPEG_FIRSTBIT,      GETBYTELEN)) &&
          (0 == strncmp(secondBit, JPEG_SECONDBIT, GETBYTELEN))) {
          return IMAGE_JPEG_JPG;
     }
     if( (0 == strncmp(firstBit, BMP_FIRSTBIT,      GETBYTELEN)) &&
          (0 == strncmp(secondBit, BMP_SECONDBIT, GETBYTELEN))) {
          return IMAGE_BMP;
     }
     if( (0 == strncmp(firstBit, PNG_FIRSTBIT,   GETBYTELEN)) &&
          (0 == strncmp(secondBit, PNG_SECONDBIT, GETBYTELEN))) {
          return IMAGE_PNG;
     }
     if( (0 == strncmp(firstBit, GIF_FIRSTBIT,   GETBYTELEN))   &&
          (0 == strncmp(secondBit, GIF_SECONDBIT, GETBYTELEN))   &&
          (0 == strncmp(thirdBit, GIF_THIRDBIT,   GETBYTELEN))   &&
          ((0 == strncmp(fifthBit, GIF_FIFTHBIT1, GETBYTELEN))   ||
          (0 == strncmp(fifthBit, GIF_FIFTHBIT2, GETBYTELEN)))) {
          return IMAGE_GIF;
     }
     if( ((0 == strncmp(firstBit, TIFF_FIRSTBIT,    GETBYTELEN))  &&
          (0 == strncmp(secondBit, TIFF_SECONDBIT,  GETBYTELEN))) ||
         ((0 == strncmp(firstBit, TIFF_FIRSTBIT2,   GETBYTELEN))  &&
          (0 == strncmp(secondBit, TIFF_SECONDBIT2, GETBYTELEN)))) {
          return IMAGE_TIFF;
     }
     if( (0 == strncmp(thirdBit, ICO_THIRDBIT, GETBYTELEN)) &&
          (0 == strncmp(fifthBit, ICO_FIFTHBIT, GETBYTELEN))){
          return IMAGE_ICO;
     }
     if( (0 == strncmp(thirdBit, TGA_THIRDBIT, GETBYTELEN)) &&
          (0 == strncmp(fifthBit, TGA_FIFTHBIT, GETBYTELEN))){
          return IMAGE_TGA;
     }
     if( (0 == strncmp(thirdBit, CUR_THIRDBIT, GETBYTELEN)) &&
          (0 == strncmp(fifthBit, CUR_FIFTHBIT, GETBYTELEN))){
          return IMAGE_CUR;
     }
     if(0 == strncmp(firstBit, PCX_FIRSTBIT, GETBYTELEN)){
          return IMAGE_PCX;
     }
     if( (0 == strncmp(firstBit, IFF_FIRSTBIT,      GETBYTELEN))   &&
          (0 == strncmp(secondBit, IFF_SECONDBIT, GETBYTELEN))   &&
          (0 == strncmp(thirdBit, IFF_THIRDBIT,   GETBYTELEN))   &&
          (0 == strncmp(forthBit, IFF_FORTHBIT,   GETBYTELEN))) {
          return IMAGE_IFF;
     }
     if( (0 == strncmp(firstBit, ANI_FIRSTBIT,   GETBYTELEN))   &&
          (0 == strncmp(secondBit, ANI_SECONDBIT, GETBYTELEN))   &&
          (0 == strncmp(thirdBit, ANI_THIRDBIT,   GETBYTELEN))   &&
          (0 == strncmp(forthBit, ANI_FORTHBIT,   GETBYTELEN))) {
          return IMAGE_ANI;
     }

     free(imagebuf);
     imagebuf = NULL;
     fclose(fd);
     return NULL;
}


LoginVerifyDialog::LoginVerifyDialog(QWidget *parent)  
    : QDialog(parent)  
{  
  
    m_inputCodeEdit = new QLineEdit(this);  
    m_okButton = new QPushButton(tr("OK"));
    
    m_picLabel = new QLabel("");
    m_infoLabel = new QLabel(tr("please input verify code"));
    
    QGridLayout* pGridLayout = new QGridLayout();  
    pGridLayout->addWidget(m_picLabel,1,1,2,2);  
    pGridLayout->addWidget(m_infoLabel,3,1,1,2);  
    pGridLayout->addWidget(m_inputCodeEdit,4,0,1,2);  
    pGridLayout->addWidget(m_okButton,4,3,1,1);   
    
    setLayout(pGridLayout);  
   
    setWindowTitle(tr("Verify code"));  
    resize(400,300);  
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(onButtonClicked()));    
}  

void LoginVerifyDialog::onButtonClicked()
{
  this->hide();
}
void LoginVerifyDialog::setImage(QString fullPath)
{
    if(strcmp("GIF", JudgmentImageByDate(fullPath.toUtf8().data())) == 0)
    {
        QMovie *movie = new QMovie(fullPath);
        m_picLabel->setMovie(movie);
        movie->start();
        m_picLabel->show();
    }else{
       m_picLabel->setPixmap(QPixmap(fullPath));
    }
}

QString LoginVerifyDialog::getVerifyString()
{
  return m_inputCodeEdit->text();
}
