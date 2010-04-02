/*********************************************************************************\
**                                                                              **
**  Copyright (C) 2008 Adam Domurad                                             **
**                                                                              **
**  This file is a part of the ENIGMA Development Environment.                  **
**                                                                              **
**                                                                              **
**  ENIGMA is free software: you can redistribute it and/or modify it under the **
**  terms of the GNU General Public License as published by the Free Software   **
**  Foundation, version 3 of the license or any later version.                  **
**                                                                              **
**  This application and its source code is distributed AS-IS, WITHOUT ANY      **
**  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS   **
**  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more       **
**  details.                                                                    **
**                                                                              **
**  You should have recieved a copy of the GNU General Public License along     **
**  with this code. If not, see <http://www.gnu.org/licenses/>                  **
**                                                                              **
**  ENIGMA is an environment designed to create games and other programs with a **
**  high-level, fully compilable language. Developers of ENIGMA or anything     **
**  associated with ENIGMA are in no way responsible for its users or           **
**  applications created by its users, or damages caused by the environment     **
**  or programs made in the environment.                                        **
**                                                                              **
\*********************************************************************************/
//Line list ---> Line list
bool collFill(collLinelist& aColl,unsigned char Blend,float aX,float aY,collLinelist& bColl,float bX,float bY, double aAngle, double bAngle, double axScale, double ayScale, double bxScale, double byScale)
{//Only takes bm_normal and bm_add
     if (aAngle==0)
     {
     unsigned int i,ii;
     long int dX,dY;
     dX=(long int)(bX-aX);
     dY=(long int)(bY-aY);
     if (Blend==bm_normal)
     {
            aColl.yOff=bColl.yOff+dY;
            collResize(aColl,bColl.x1List.size());
            for (i=0;i<aColl.x1List.size();i++)
            {
            aColl.x1List[i]=bColl.x1List[i]+dX;
            aColl.x2List[i]=bColl.x2List[i]+dX;
            }
     }
     else
     {
           if (aColl.yOff>bColl.yOff+dY)
           {
           aColl.x1List.insert(aColl.x1List.begin(),aColl.yOff-bColl.yOff-dY,0);
           aColl.x2List.insert(aColl.x2List.begin(),aColl.yOff-bColl.yOff-dY,0);
           aColl.yOff=bColl.yOff;
           ii=0;
           }
           else
           ii=bColl.yOff-aColl.yOff+dY;//distance from first yOff to second yOff
            collResize(aColl,std::max((long)aColl.x1List.size(),(long)bColl.x1List.size()+(long)ii));
            for (i=ii;i<bColl.x1List.size()+ii;i++)
            {
            aColl.x1List[i]=bColl.x1List[i-ii]+dX;
            aColl.x2List[i]=bColl.x2List[i-ii]+dX;

            }
     }
     }
     return 0;
}

//Custom --> Custom
bool collFill(collCustom& aColl,unsigned char Blend,float aX,float aY,collCustom& bColl,float bX,float bY, double aAngle, double bAngle){
     float i,ii,minX,maxX,minY,maxY;
     unsigned int aRef,bRef;
     unsigned char bMod,cMod;bool dir;
     double rot[8],aSin,bSin,bCos,aCos;
     aAngle/=57.295779513082320876798154814105;
     bAngle/=57.295779513082320876798154814105;
     aSin=sin(aAngle);
     aCos=cos(aAngle);
     bSin=sin(bAngle);
     bCos=cos(bAngle);
     if (Blend==bm_normal)
     collFill(aColl,0);
     aX+=aColl.xOff;
     aY+=aColl.yOff;
     aX+=aSin*aColl.yOrigin-aCos*aColl.xOrigin+aColl.xOrigin;
     aY+=aColl.yOrigin-aCos*aColl.yOrigin-aSin*aColl.xOrigin;
     bX+=bSin*bColl.yOrigin-bCos*bColl.xOrigin+bColl.xOrigin;
     bY+=bColl.yOrigin-bCos*bColl.yOrigin-bSin*bColl.xOrigin;
     bX+=bColl.xOff-aX;
     bY+=bColl.yOff-aY;
     if ((aAngle == bAngle)&&(aColl.yOrigin==0))
     {
     minX=(int)std::max((float)0,bX);
     minY=(int)std::max((float)0,bY);
     maxX=(int)std::min((float)aColl.xDim,bX+bColl.xDim)-1;
     maxY=(int)std::min((float)aColl.yDim,bY+bColl.yDim);
     cMod=(unsigned char)(abs((int)bX)&7);
     dir=(0<bX);
     for (ii=(int)minY;ii<(int)maxY;ii++)
                {
                    for (i=((int)minX)>>3;i<=((int)maxX)>>3;i++)
                    {
                        aRef=(unsigned int)(i  +  (ii) * (aColl.byteDim));
                        bRef=(unsigned int)(i - ((int)minX>>3)  +  (ii - bY) * (bColl.byteDim));
                        bMod=bColl.cMask[bRef];
                        ///CASE 1
                        if (i==(int)minX>>3)
                        {
                           if ((cMod!=0) && ((i*8+7)<maxX))
                           {
                              if (dir)
                                 bMod = /*(bColl.cMask[bRef+1]>>8-cMod)|*/(bMod << cMod);
                              else
                                 bMod = (bColl.cMask[bRef+1]<<8-cMod)|(bMod >> cMod);
                           }
                           bMod&=255>>(((int)(-maxX-1)&7)*((i*8+8)>(int)maxX));
                           bMod&=255<<((int)(minX)&7);
                           switch (Blend)
                           {
                                  case bm_normal:
                                  aColl.cMask[aRef]=bMod;break;
                                  case bm_add:
                                  aColl.cMask[aRef]|=bMod;break;
                                  case bm_subtract:
                                  aColl.cMask[aRef]&=~(bMod);break;
                                  case bm_negate:
                                  aColl.cMask[aRef]^=bMod;break;
                           }
                        }
                        else
                        {///CASE 2
                        if (i==(int)maxX>>3)
                           {
                           if ((cMod!=0))
                           {
                              if (dir)
                                 bMod =(bColl.cMask[bRef-1]>>8-cMod)|(bMod << cMod);
                              else
                                 bMod >>= cMod;
                           }
                           bMod&=255u>>((int)(-maxX-1)&7);
                           switch (Blend)
                           {
                                  case bm_normal:
                                  aColl.cMask[aRef]=bMod;break;
                                  case bm_add:
                                  aColl.cMask[aRef]|=bMod;break;
                                  case bm_subtract:
                                  aColl.cMask[aRef]&=~(bMod);break;
                                  case bm_negate:
                                  aColl.cMask[aRef]^=bMod;break;
                           }
                           }///CASE 3
                        else
                            {
                           if ((cMod!=0) && ((i*8+7)<(int)maxX))
                           {
                              if (dir)
                                 bMod =(bColl.cMask[bRef-1]>>8-cMod)|(bMod << cMod);
                              else
                                 bMod =(bColl.cMask[bRef+1]<<8-cMod)|(bMod >> cMod);
                           }
                           switch (Blend)
                           {
                                  case bm_normal:
                                  aColl.cMask[aRef]=bMod;break;
                                  case bm_add:
                                  aColl.cMask[aRef]|=bMod;break;
                                  case bm_subtract:
                                  aColl.cMask[aRef]&=~(bMod);break;
                                  case bm_negate:
                                 aColl.cMask[aRef]^=bMod;break;
                           }
                            }
                        }
                        }

                }}//end NONROTATED
                     else
         {//begin ROTATED

         //temp holding places for X
         rot[0]=0;
         rot[1]=aCos*(aColl.xDim)-aSin*(aColl.yDim);
         rot[2]=-aSin*(aColl.yDim);
         rot[3]=aCos*(aColl.xDim);
         rot[4]=bCos*(bColl.xDim)+bX;
         rot[5]=bX;
         rot[6]=bCos*(bColl.xDim)-bSin*(bColl.yDim)+bX;
         rot[7]=-bSin*(bColl.yDim)+bX;

         //end temp holding places for X
         aX=(int)floor(std::max(std::min(rot[0],std::min(rot[1],std::min(rot[2],rot[3]))),std::min(rot[7],std::min(rot[4],std::min(rot[5],rot[6])))));
         aY=(int)ceil(std::min(std::max(rot[0],std::max(rot[1],std::max(rot[2],rot[3]))),std::max(rot[7],std::max(rot[4],std::max(rot[5],rot[6])))));
         minX=std::min(aX,aY);
         maxX=std::max(aX,aY);
         rot[0]=0;
         rot[1]=aCos*(aColl.yDim)+aSin*(aColl.xDim);
         rot[2]=aCos*(aColl.yDim);
         rot[3]=aSin*(aColl.xDim);
         rot[4]=bY;
         rot[5]=bCos*(bColl.yDim)+bSin*(bColl.xDim)+bY;
         rot[6]=bCos*(bColl.yDim)+bY;
         rot[7]=bSin*(bColl.xDim)+bY;
         //end temp holding places for Y
         aX=(int)floor(std::min(std::min(rot[0],std::min(rot[1],std::min(rot[2],rot[3]))),std::min(rot[7],std::min(rot[4],std::min(rot[5],rot[6])))));
         aY=(int)ceil(std::max(std::max(rot[0],std::max(rot[1],std::max(rot[2],rot[3]))),std::max(rot[7],std::max(rot[4],std::max(rot[5],rot[6])))));
         minY=std::min(aX,aY);
         maxY=std::max(aX,aY);
              for (ii=minY;ii<=maxY;ii+=0.5)
                {
                    for (i=minX;i<=maxX;i+=0.5)
                    {

                    dir=0;
                    rot[0]=(int)round(aCos*(i)+aSin*(ii));
                    rot[1]=(int)round(aCos*(ii)-aSin*(i));
                       if ((rot[0]>=0) && (rot[0] <aColl.xDim))
                       {
                       if ((rot[1]>=0) && (rot[1] <aColl.yDim))
                       {
                          rot[2]=(int)round(bCos*(i-bX)+bSin*(ii-bY));
                          rot[3]=(int)round(bCos*(ii-bY)-bSin*(i-bX));
                          if ((rot[2]>=0) && (rot[2] <aColl.xDim))
                       {
                          if ((rot[3]>=0) && (rot[3] <aColl.yDim))
                          {
                          if (collbitGet(bColl,(unsigned short)rot[2],(unsigned short)rot[3]))
                             {
                             switch (Blend)
                             {
                                          case bm_normal:
                                          case bm_add:
                                          collbitSet(aColl,(unsigned short)rot[0],(unsigned short)rot[1],1);break;
                                          case bm_subtract:
                                          collbitSet(aColl,(unsigned short)rot[0],(unsigned short)rot[1],0);break;
                                          case bm_negate:
                                          collbitNegate(aColl,(unsigned short)rot[0],(unsigned short)rot[1]);break;
                             }
                          }
                          }
                       }
                       }
                       }
                    }
                }
     } return 0;}
//Custom --> Linelist
bool collFill(collCustom& aColl,unsigned char Blend,float aX,float aY,collLinelist& bColl,float bX,float bY,double Angle){
     int dX,dY; unsigned int i, ii;
     float aX1,aX2;
     aX+=aColl.xOff;
     aY+=aColl.yOff;
     if (Blend==bm_normal)
     collFill(aColl,0);
     if (Angle==(int)(Angle))
     {
     unsigned char bitMod, byteRef;
     if (((int)(Angle)%360)==0)//nonrotated case
     {
     dX=(int)(bX-aX);
     dY=(int)(bY+bColl.yOff-aY);
            for (ii=0;ii<aColl.yDim;ii++)
            {
                if ((ii>=(unsigned int)dY)&&(ii<(unsigned int)(dY+bColl.x1List.size())))
                {
                 aX1=std::max((float)0,std::min((float)aColl.xDim,bColl.x1List[ii-dY]+dX));
                 aX2=std::max((float)0,std::min((float)aColl.xDim,bColl.x2List[ii-dY]+dX));
                 #include "linear_fill.h"
                }

            }
            return 1;
     }
     }
     double aSin,aCos;
     Angle/=57.295779513082320876798154814105;
     aSin=sin(Angle);
     aCos=cos(Angle);
     aX+=aSin*aColl.yOrigin-aCos*aColl.xOrigin+aColl.xOrigin;
     aY+=aColl.yOrigin-aCos*aColl.yOrigin-aSin*aColl.xOrigin;
     for (ii=0;ii<aColl.yDim;ii++)
            {
            for (i=0;i<aColl.xDim;i++)
            {
            /*X-Value*/aX1=i*aCos-ii*aSin;
            /*Y-Value*/aX2=i*aSin+ii*aCos;
            if ((aX2>=bColl.yOff)&&((unsigned int)(aX2-bColl.yOff)<bColl.x1List.size()))
                {
                if (collPoint(bColl,aX1,aX2))
                {
                switch (Blend)
                {
                case bm_normal:
                case bm_add:
                collbitSet(aColl,(unsigned short)i,(unsigned short)ii,1);break;
                case bm_subtract:
                collbitSet(aColl,(unsigned short)i,(unsigned short)ii,0);break;
                case bm_negate:
                collbitNegate(aColl,(unsigned short)i,(unsigned short)ii);break;
                }
                }

                }
            }
            }
   return 0;
}
//Linelist --> Custom
bool collFill(collLinelist& bColl,float bX,float bY,unsigned char Blend,collCustom& aColl,float aX,float aY,double Angle){
     int dX,dY; unsigned int i,ii;
     float aX1,aX2;
     aX+=aColl.xOff;
     aY+=aColl.yOff;
     if (Blend==bm_normal)
     collFill(aColl,0);
     if (Angle==(int)(Angle))
     {
     //unsigned char bitMod, byteRef;
     if (((int)(Angle)%360)==0)//nonrotated case
     {
     dX=(int)(bX-aX);
     dY=(int)(bY+bColl.yOff-aY);
            for (ii=0;ii<(unsigned int)aColl.yDim;ii++)
            {
                if ((ii>=(unsigned int)dY)&&(ii<(unsigned int)(dY+bColl.x1List.size())))
                {
                 aX1=std::max((float)0,std::min((float)aColl.xDim,bColl.x1List[ii-dY]+dX));
                 aX2=std::max((float)0,std::min((float)aColl.xDim,bColl.x2List[ii-dY]+dX));
                }

            }
            return 1;
     }
     }
     double aSin,aCos;
     Angle/=57.295779513082320876798154814105;
     aSin=sin(Angle);
     aCos=cos(Angle);
     aX+=aSin*aColl.yOrigin-aCos*aColl.xOrigin+aColl.xOrigin;
     aY+=aColl.yOrigin-aCos*aColl.yOrigin-aSin*aColl.xOrigin;
     for (ii=0;ii<aColl.yDim;ii++)
            {
            for (i=0;i<aColl.xDim;i++)
            {
            /*X-Value*/aX1=i*aCos-ii*aSin;
            /*Y-Value*/aX2=i*aSin+ii*aCos;
            if ((aX2>=bColl.yOff)&&((unsigned int)(aX2-bColl.yOff)<(unsigned int)bColl.x1List.size()))
                {
                if (collPoint(bColl,aX1,aX2))
                {
                switch (Blend)
                {
                case bm_normal:
                case bm_add:
                collbitSet(aColl,(unsigned short)i,(unsigned short)ii,1);break;
                case bm_subtract:
                collbitSet(aColl,(unsigned short)i,(unsigned short)ii,0);break;
                case bm_negate:
                collbitNegate(aColl,(unsigned short)i,(unsigned short)ii);break;
                }
                }

                }
            }
            }
   return 0;
}

