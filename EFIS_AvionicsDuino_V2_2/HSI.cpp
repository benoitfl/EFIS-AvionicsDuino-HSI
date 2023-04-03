/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //                                                 HSI.cpp
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  
HSI.cpp is part of EFIS_Avionicsduino_V2.2
EFIS_Avionicsduino_V2.2 is free software    

MIT License (MIT)

Copyright (c) 2023 AvionicsDuino - benjamin.fremond@avionicsduino.com
Contributors : Benjamin Frémond - Gabriel Consigny - Michel Riazuelo - BENOIT Florent

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 *****************************************************************************************************************************/
#include "HSI.h"

HSI::HSI(RA8875 * ptr)
{
  ptr_tft = ptr;
  LD = Cx + DL;
  LG = Cx - DL;
  LH = Cy - DH;
  LB = Cy + DH;
  largeur = DL * 2;
  hauteur = DH * 2;
  Ybille = Cy + DH - Rbille - 2;
  RayonGraduations = (hauteur / 3) + 20;
  racpiai = RayonGraduations - 19;
}

void HSI::begin()
{
  ptr_tft->setFontScale (1);
  ptr_tft->clearMemory();
  // Initialise l'utilisation des layers et dessine le ciel et la terre sur la layer2
  ptr_tft->writeTo(L2);
  ptr_tft->fillWindow(RA8875_BLACK);
  //ptr_tft->fillRect (Cx - DL, Cy - DH, DL * 2, DH, OCRE);
  //ptr_tft->fillRect (Cx - DL, Cy, DL * 2, DH, BLEUCIEL);
  ptr_tft->setTransparentColor(RA8875_PINK);
  ptr_tft->writeTo(L1);
  // Et maintenant, on affiche la Layer 1, on lui attribue un fond rose, puis on la superpose sur la Layer 2 par application d'un effet "TRANSPARENT"
  ptr_tft->layerEffect(LAYER1);
  ptr_tft->fillWindow(RA8875_PINK);
  ptr_tft->layerEffect(TRANSPARENT);
  // Traçage de l'arc fixe et des graduations d'inclinaison de l'horizon artificiel, de la maquette, et du cadre de la bille
  redessine();
  // On repasse en fonte par défaut
  ptr_tft->setFontDefault();
  ptr_tft->setFontScale (0);
}

void HSI::dessine(float aHDG, float aTrack, float aYaw, float aOffsetBille, bool aMenuOUvert)
{
  // ******************************************************************************************** On affiche d'abord sur le premier plan (layer 1) toutes les parties mobiles **************************************************************************************
  ptr_tft->writeTo(L1);
  ptr_tft->setTextColor(RA8875_WHITE, RA8875_PINK);
  ptr_tft->setFontDefault();
  ptr_tft->setFontScale (0);
  for (uint8_t n = 0; n <= 36; n++)
  {
    ptr_tft->drawLineAngle  (Cx, Cy, (n*10)+aHDG, RayonGraduations - 5, RA8875_WHITE);
  }
  for (uint8_t n = 0; n <= 12; n++)
  {
    ptr_tft->drawLineAngle (Cx, Cy, (n*30)+aHDG, RayonGraduations + 2, RA8875_WHITE);
  }
  ptr_tft->fillCircle (Cx, Cy, RayonGraduations - 18, RA8875_PINK);
  // ****************************************************** On affiche la bille sur la Layer 1 (premier plan) *******************************************************
  if (aMenuOUvert == false)
  {
    ptr_tft->writeTo(L1);
    bille (offsetBilleOld, RA8875_PINK);
    bille(aOffsetBille, RA8875_WHITE);
  }

  // ********************* On enregistre toutes les positions clés pour pouvoir effaccer les anciennes graduations au prochain passage dans Loop. *******************************************
  curseurGxold3 = curseurGx3;
  curseurGyold3 = curseurGy3;
  offsetBilleOld = aOffsetBille;
  Mxold = Mx;
  Myold = My;
}

void HSI::redessine()
{
  ptr_tft->layerEffect(LAYER2); delay(1);
  for (uint8_t n = 0; n <= 36; n++)
  {
    ptr_tft->drawLineAngle  (Cx, Cy, (n*10), RayonGraduations - 5, RA8875_WHITE);
  }
  for (uint8_t n = 0; n <= 12; n++)
  {
    ptr_tft->drawLineAngle (Cx, Cy, (n*30), RayonGraduations + 2, RA8875_WHITE);
  }
  ptr_tft->fillCircle (Cx, Cy, RayonGraduations - 18, RA8875_PINK);
  // Dessin de l'arc fixe
  ptr_tft->drawArc (Cx, Cy, RayonGraduations - 15, 2, 300, 60, RA8875_WHITE);
  ptr_tft->layerEffect(TRANSPARENT);



  // Dessin du cadre de la bille
  ptr_tft->drawArc (Cx - Rbille * 8 - 1, Ybille, Rbille + 2, 1, 180, 360, RA8875_BLACK);
  ptr_tft->drawArc (Cx + Rbille * 8 + 1, Ybille, Rbille + 2, 1, 0, 180, RA8875_BLACK);
  ptr_tft->drawLine (Cx - Rbille * 8 - 1, Ybille - Rbille - 1, Cx + Rbille * 8 + 1, Ybille - Rbille - 1, RA8875_BLACK);
  ptr_tft->drawLine (Cx - Rbille * 8 - 1, Ybille + Rbille + 1, Cx + Rbille * 8 + 1, Ybille + Rbille + 1, RA8875_BLACK);

  // Dessin partiel du cadre graphique du vario
  ptr_tft->setFontDefault();
  ptr_tft->setFontScale (0);
  ptr_tft->setTextColor (RA8875_WHITE, RA8875_PINK);
  byte j = 0;
  // ecran de 480*272 20 ***** ecran 800*480  8
  for (uint16_t i = 8; i <= Sy; i = i + 58) 
  {
    ptr_tft->setCursor(Sx-10, i - 9);
    ptr_tft->print(abs(4 - j));
    j++;
  }
  ptr_tft->drawFastVLine (Sx-20, 8, Sy-16, RA8875_WHITE);

  // Dessin des cadres d'affichage des altitudes
  ptr_tft->fillRect(Sx-70, Sy/2-(38/2), 50, 38, RA8875_BLACK);
  ptr_tft->drawRect(Sx-70, Sy/2-(38/2), 50, 38, RA8875_LIGHT_GREY);
  //ptr_tft->drawRect(410,156,50,20, RA8875_LIGHT_GREY);
  //ptr_tft->drawRect(410,177,50,20, RA8875_LIGHT_GREY);

  // Dessin des cadres d'affichage des vitesses
  ptr_tft->fillRect(2, Sy/2-(38/2), 60, 38, RA8875_BLACK);
  ptr_tft->drawRect(2, Sy/2-(38/2), 60, 38, RA8875_LIGHT_GREY);
  //ptr_tft->drawRect(2,96,60,20, RA8875_LIGHT_GREY);

  // Affichage des légendes fixes
  ptr_tft->setCursor((Sx/2)+29, 19); ptr_tft->print("AHRS TRK");
  ptr_tft->setCursor((Sx/2)-88, 19); ptr_tft->print("MAG HDG");
  ptr_tft->setCursor(2, 82); ptr_tft->print("GPS");
  ptr_tft->setCursor(2, 97); ptr_tft->print("TAS");
  ptr_tft->setCursor(Sx-102, 157); ptr_tft->print("AltP");
  ptr_tft->setCursor(Sx-102, 172); ptr_tft->print("AltD");
  ptr_tft->setCursor(Sx-102, 187); ptr_tft->print("AltG");
  ptr_tft->setCursor(Sx-102, 206); ptr_tft->print("QNH");
  ptr_tft->setCursor (71, (Sy/2)); ptr_tft->print("IAT:");
  ptr_tft->setCursor (71, (Sy/2)+15); ptr_tft->print("OAT:");
  ptr_tft->setCursor (71, (Sy/2)+30); ptr_tft->print("RH :");
  ptr_tft->setCursor (71, (Sy/2)+45); ptr_tft->print("DPT:");
  ptr_tft->setCursor (2, 30); ptr_tft->print("Wdir:");
  ptr_tft->setCursor (2, 45); ptr_tft->print("Wspd:");

  // Dessin des cadres d'affichage de la route et du cap magnétique
  ptr_tft->fillRect((Sx/2)+29, 1, 60, 22, RA8875_BLACK);
  ptr_tft->drawRect((Sx/2)+29, 1, 60, 22, RA8875_LIGHT_GREY);
  ptr_tft->fillRect((Sx/2)-88, 1, 60, 22, RA8875_BLACK);
  ptr_tft->drawRect((Sx/2)-88, 1, 60, 22, RA8875_LIGHT_GREY);

  // Dessin des cadres d'affichages températures, G, altP&D, varios (dans cet ordre)
  ptr_tft->drawRect(68, Sy/2, 80, 65, RA8875_LIGHT_GREY);
  ptr_tft->drawRect(Sx-120, 66, 87, 49, 0x7BEF);
  ptr_tft->drawRect(Sx-106, 156, 84, 50, RA8875_LIGHT_GREY);
  ptr_tft->drawRect(Sx-124, 2, 98, 63, 0x7BEF);
}

// ************************************************************************************** Quelques fonctions primitives graphiques ********************************************************************************************

void HSI::fillPentagone( int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint16_t color)
{
  ptr_tft->fillTriangle (x0, y0, x1, y1, x4, y4, color);
  ptr_tft->fillTriangle (x1, y1, x2, y2, x4, y4, color);
  ptr_tft->fillTriangle (x2, y2, x3, y3, x4, y4, color);
}

void HSI::fillQuadrilatere ( int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
  ptr_tft->fillTriangle (x0, y0, x1, y1, x3, y3, color);
  ptr_tft->fillTriangle (x1, y1, x2, y2, x3, y3, color);
}

// ********************************************************************************************************** Fonction liée à la bille ******************************************************************************************************************************
void HSI::bille( int8_t ofs, uint16_t couleur)
{
  ptr_tft->drawCircle (Cx + ofs, Ybille, Rbille, couleur);
  ptr_tft->drawCircle (Cx + ofs, Ybille, 5, couleur);
  ptr_tft->fillRect (Cx - Rbille - 4, LH + hauteur - Rbille * 2 - 2, 2, Rbille * 2 + 1, RA8875_BLACK);
  ptr_tft->fillRect (Cx + Rbille + 3, LH + hauteur - Rbille * 2 - 2, 2, Rbille * 2 + 1, RA8875_BLACK);
}
