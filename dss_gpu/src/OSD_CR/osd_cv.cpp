#include"osd_cv.h"
#include "statCtrl.h"

#define PI 3.1415926

 CvScalar  GetcvColour(int colour)
{

	switch(colour)
		{
		case 0:
			return cvScalar(0,0,0,0);
			break;
		case 1:
			return cvScalar(0,0,0,255);
			break;
		case 2:
			return cvScalar(255,255,255,255);
			break;
		case 3:
			return cvScalar(0,0,255,255);
			break;
		case 4:
			return cvScalar(0,255,255,255);
			break;
		case 5:
			return cvScalar(255,0,0,255);
			break;
		case 6:
			return cvScalar(0,255,0,255);
			break;
		default:
			return cvScalar(255,255,255,255);
			break;
		}


}
#if 1
void DrawcvDashline(Mat frame,Line_Param_fb *lineparm,int linelength,int dashlength)
{
	int startx, starty, endx, endy;
	  int i=0;
	  int flagx=1;
	  int flagy=1;
	   int totallengthy;
	   if(lineparm==NULL)
	   	{
			return;
	   	}
	   startx=lineparm->x-lineparm->width/2;
	   starty=lineparm->y-lineparm->height/2;
	   endx=lineparm->x+lineparm->width/2;
	   starty=lineparm->y+lineparm->height/2;
	  int totallength=dashlength+linelength;  
  	  int nCount=abs(endx-startx)/totallength;//  
  	  CvScalar colour=GetcvColour(lineparm->frcolor);
  	  if(nCount!=0)
  	        totallengthy=abs(endy-starty)/nCount;//  
  	  
  	  Point start,end;//start and end point of each dash  
  	  if(startx>endx)
  	  	{
			flagx=-1;
  	  	}
	   if(starty>endy)
  	  	{
			flagy=-1;
  	  	}
	if(startx==endx)
		{
			nCount=abs(endy-starty)/totallength;// 
			totallengthy=abs(endy-starty)/nCount;
			for (int i=0;i<nCount;i++)
		   	 {  
		      		end.x=startx;
		      		start.x=startx; 
		      		start.y=starty+i*totallengthy*flagy;  
				end.y=starty+(i+1)*totallengthy*flagy-dashlength*flagy;//draw left dash line  
				 line(frame, start, end, colour, 1, 8, 0 ); 
		  	  }  
			return ;
		}
	for (int i=0;i<nCount;i++)
   	 {  
      		end.x=startx+(i+1)*totallength*flagx-dashlength*flagx;//draw top dash line  
      		start.x=startx+i*totallength*flagx;  
      		start.y=starty+i*totallengthy*flagy;  
		end.y=starty+(i+1)*totallengthy*flagy-dashlength*flagy;//draw left dash line  
		 line(frame, start, end, colour, 1, 8, 0 ); 
  	  }  


}

void drawdashlinepri(Mat frame,int startx,int starty,int endx,int endy,int linelength,int dashlength,int colour1)
{
	  int i=0;
	  int flagx=1;
	  int flagy=1;
	  double totallengthy=0.0;
	  double  totallengthx=0.0;
	  int totallength=dashlength+linelength;  
	 // float len=sqrt((endy-starty)*(endy-starty)+(endx-startx)*(endx-startx));
	 int len=abs(endy-starty)+abs(endx-startx);
  	  double nCount=len/totallength;// 
  	  CvScalar colour=GetcvColour(colour1);//Getcvcolour(extInCtrl.DispColor[extInCtrl.SensorStat]);
  	  totallengthx=abs(endx-startx)*1.0/nCount;
  	  totallengthy=abs(endy-starty)*1.0/nCount;
  	  
  	  Point start,end;//start and end point of each dash  
  	  if(startx>endx)
  	  	{
			flagx=-1;
  	  	}
	   if(starty>endy)
  	  	{
			flagy=-1;
  	  	}
	if(startx==endx)
		{

		
			for (int i=0;i<nCount;i=i+2)
		   	 {  
		      		end.x=startx;
		      		start.x=startx; 
		      		start.y=cvRound(starty+i*totallengthy*flagy);
		      		end.y=cvRound(starty+(i+1)*totallengthy*flagy);//draw left dash line
				 line(frame, start, end, colour, 1, 8, 0 ); 
		  	  }  
			return ;
		}
	if(starty==endy)
		{
			for (int i=0;i<nCount;i=i+2)
		   	 {  
		      		start.x=cvRound(startx+i*totallengthx*flagx);
		      		end.x=cvRound(startx+(i+1)*totallengthx*flagx); 
		      		start.y=starty;
				end.y=starty; 
				 line(frame, start, end, colour, 1, 8, 0 ); 
		  	  }  
			return ;


		}

	
	  
  	  
	for (int i=0;i<nCount;i=i+2)
   	 {  
      		end.x=cvRound(startx+(i+1)*totallengthx*flagx);//draw top dash line  
      		start.x=cvRound(startx+i*totallengthx*flagx);  
      		start.y=cvRound(starty+i*totallengthy*flagy);  
		end.y=cvRound(starty+(i+1)*totallengthy*flagy);//draw left dash line  
  
		 line(frame, start, end, colour, 1, 8, 0 ); 
  	  }  


}

void DrawcvDashliner(Mat frame,int startx,int starty,int endx,int endy,int linelength,int dashlength,int frcolor)
{

	  int i=0;
	  int flagx=1;
	  int flagy=1;
	   int totallengthy;


	  int totallength=dashlength+linelength;  
  	  int nCount=abs(endx-startx)/totallength;//  
  	  CvScalar colour=GetcvColour(frcolor);
  	  if(nCount!=0)
  	        totallengthy=abs(endy-starty)/nCount;//  
  	  
  	  Point start,end;//start and end point of each dash  
  	  if(startx>endx)
  	  	{
			flagx=-1;
  	  	}
	   if(starty>endy)
  	  	{
			flagy=-1;
  	  	}
	if(startx==endx)
		{
			nCount=abs(endy-starty)/totallength;// 
			totallengthy=abs(endy-starty)/nCount;
			for (int i=0;i<nCount;i++)
		   	 {  
		      		end.x=startx;
		      		start.x=startx; 
		      		start.y=starty+i*totallengthy*flagy;  
				end.y=starty+(i+1)*totallengthy*flagy-dashlength*flagy;//draw left dash line  
				 line(frame, start, end, colour, 1, 8, 0 ); 
		  	  }  
			return ;
		}
	for (int i=0;i<nCount;i++)
   	 {  
      		end.x=startx+(i+1)*totallength*flagx-dashlength*flagx;//draw top dash line  
      		start.x=startx+i*totallength*flagx;  
      		start.y=starty+i*totallengthy*flagy;  
		end.y=starty+(i+1)*totallengthy*flagy-dashlength*flagy;//draw left dash line  
		 line(frame, start, end, colour, 1, 8, 0 ); 
  	  }  


}


void DrawcvDashcross(Mat frame,Line_Param_fb *lineparm,int linelength,int dashlength)
{
	int centx, centy, width;
	if(lineparm==NULL)
		{
			return;
		}
	centx=lineparm->x;
	centy=lineparm->y;
	width=lineparm->width;
	int startx=centx-width/2;
	int starty=centy;
	int endx=centx+width/2;
	int endy=centy;
	//DrawcvDashline(frame,lineparm,linelength,dashlength);
	drawdashlinepri(frame,startx,starty,endx,endy,linelength,dashlength,lineparm->frcolor);
	//drawdashline(frame,startx,starty,endx,endy,linelength,dashlength);

	startx=centx;
	starty=centy-width/2;
	endx=centx;
	endy=centy+width/2;
	//DrawcvDashline(frame,lineparm,linelength,dashlength);
	drawdashlinepri(frame,startx,starty,endx,endy,linelength,dashlength,lineparm->frcolor);
	

}

void DrawcvLine(Mat frame,Osd_cvPoint *start,Osd_cvPoint *end,int frcolor,int linew)
{
	Point pt1,pt2,center;
	 CvScalar colour=GetcvColour(frcolor);
	pt1.x=start->x;
	pt1.y=start->y;
	pt2.x=end->x;
	pt2.y=end->y;
	line(frame, pt1, pt2, colour, linew, 8, 0 );
}


void DrawjsCompass(Mat frame,CFOV * fovOsdCtrl)
{
	CFOV* cthis = NULL;
	cthis = fovOsdCtrl;
	
	int linew = 1;
	Point center;
	Point start,end;
	Osd_cvPoint start1,end1;
	
	center.x =650;
	center.y = 130;
	int radius = 28;
	int lcolor = 0;
	CvScalar color=GetcvColour(cthis->frcolor);
		
	circle(frame,center,radius ,color,1,8,0);
	lcolor = cthis->frcolor;
	
	//draw the interval
	start.x = center.x ;
	start.y = center.y - radius;
	end.x = start.x ;
	end.y = start.y + 2*radius;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, 0, 5); 	//shu
	
	start.x = center.x - radius;
	start.y = center.y ;
	end.x = start.x + 2*radius;
	end.y = start.y ;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, 0, 5);		//heng
	
	start.x = center.x - 23;
	start.y = center.y -40;
	end.x = center.x + 23;
	end.y = center.y + 40 ;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, 0, 5);		//xie
	start.x = center.x + 23;
	start.y = center.y  - 40;
	end.x  = center.x  - 23;
	end.y  = center.y + 40 ;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, 0, 5);		//xie
	
	
	start.x = center.x - 33;
	start.y = center.y -20;
	end.x = center.x + 33;
	end.y = center.y + 20 ;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, 0, 5);		//xie
	start.x = center.x + 33;
	start.y = center.y - 20;
	end.x  = center.x  -  33;
	end.y  = center.y + 20 ;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, 0, 5);		//xie

	//wai mian si tiao xian
	start.x = center.x - radius;
	start.y = center.y ;
	end.x  =  start.x - 20;
	end.y  =  start.y;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, lcolor, 1);		
	
	start.x = center.x + radius;
	start.y = center.y ;
	end.x  =  start.x + 20;
	end.y  =  start.y;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, lcolor, 1);	

	start.x = center.x;
	start.y = center.y - radius;
	end.x  =  start.x;
	end.y  =  start.y - 20;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, lcolor, 1);	

	start.x = center.x;
	start.y = center.y + radius;
	end.x  =  start.x ;
	end.y  =  start.y + 20;
	start1.x = start.x;
	start1.y = start.y;
	end1.x = end.x;
	end1.y = end.y;
	DrawcvLine(frame, &start1, &end1, lcolor, 1);	

	//zhishizhen
	DrawjsXLine(frame,cthis);

	return ;
}


void DrawjsXLine(Mat frame,CFOV * fovOsdCtrl)
{
	double theta;
	int value= 0;
	CFOV * cthis = NULL;
	cthis = fovOsdCtrl;
	value = cthis->theta;
	
	value %= 360;
	theta = (value*PI)/180;
	int len = 50;
	int linew = 1;
	Point center,end;
	center.x = 650;
	center.y = 130;

	end.x = center.x + len*(sin(theta));
	end.y = center.y - len*(cos(theta));

	 CvScalar colour=GetcvColour(cthis->frcolor);
	 line(frame, center, end, colour, linew, 8, 0 );
	 

}



void DrawjsOblinqueLine(Mat frame,Line_Param_fb* lineparm)
{
	
	
	return ;
}


void DrawjsRuler(Mat frame,CFOV * fovOsdCtrl)
{
	CFOV* cthis = fovOsdCtrl;
	
	Osd_cvPoint start;
	Osd_cvPoint end;
	int linew = 1;
	int i;

	int CenterX=cthis->fovX;
	int CenterY=cthis->fovY-50;
	int Len=15;
	
	int StepGap=50;

	if(isFovMachineGun())
		StepGap = 30;
	else if(isFovGrenadeGas())
		StepGap = 20;
	else if(isFovGrenadeKill())
		StepGap = 25;
	if(isFovSmall())
		StepGap *= 1.2;


	for(i = -3;i<=3;i++)
	{
		start.x = CenterX + i*StepGap ;
		start.y = CenterY;
		end.x = start.x ;
		end.y = start.y -2*Len;
		DrawcvLine(frame, &start, &end, cthis->frcolor, linew);
	}

	for(i = -3;i<=2;i++)
	{
		start.x = CenterX + i*StepGap + StepGap/2;
		start.y = CenterY;
		end.x = start.x ;
		end.y = start.y -Len;
		DrawcvLine(frame, &start, &end, cthis->frcolor, linew);
	}

	return ;
}



void DrawjsCross(Mat frame,CFOV * fovOsdCtrl)
{	
	int len =	15;
	int linew = 1;
	int offlen = 0;
	if(fovOsdCtrl == NULL)
		return;
	CFOV * cthis ;
	cthis = fovOsdCtrl;
	Osd_cvPoint start;
	Osd_cvPoint end;
	int flag = 0 ;  // 0 jiguang  chang  1 ren duan

	if(gMeasureType ||finish_laser_measure)
		flag = 1;
	
	if(isFovSmall())
		len *= 1.2;
	
	if(flag)
		offlen = 0;
	else
		offlen = 20;
	
	//ren

	start.x = cthis->fovX;
	start.y = cthis->fovY;
	end.x = start.x;
	end.y = start.y;
	DrawcvLine(frame, &start, &end, cthis->frcolor, 3);

	start.x = cthis->fovX - cthis->crosswidth/2 - offlen;
	start.y = cthis->fovY;
	end.x = start.x +len + offlen;
	end.y = start.y;
	DrawcvLine(frame, &start, &end,cthis->frcolor, linew);

	start.x = cthis->fovX + cthis->crosswidth/2 + offlen;
	start.y = cthis->fovY;
	end.x = start.x -len - offlen;
	end.y = start.y;
	DrawcvLine(frame, &start, &end, cthis->frcolor, linew);
		
	start.x = cthis->fovX;
	start.y = cthis->fovY - cthis->crossheight/2;
	end.x = start.x;
	end.y = start.y + len;
	DrawcvLine(frame, &start, &end, cthis->frcolor, linew);

	start.x = cthis ->fovX;
	start.y = cthis ->fovY + cthis->crossheight/2;
	end.x = start.x;
	end.y = start.y -len;
    	DrawcvLine(frame, &start, &end, cthis->frcolor, linew);
	
	start.x = cthis->fovX - cthis->crosswidth/2;
	start.y = cthis->fovY - len/2;
	end.x = start.x;
	end.y = start.y + len;
	DrawcvLine(frame,&start,&end, cthis->frcolor,linew);

	start.x = cthis->fovX + cthis->crosswidth/2;
	start.y = cthis->fovY - len/2;
	end.x = start.x;
	end.y = start.y + len;
	DrawcvLine(frame,&start,&end, cthis->frcolor,linew);
	
}


void DrawjsGrenadeLoadOK(Mat frame,CFOV * fovOsdCtrl)
{
	Osd_cvPoint start;
	Osd_cvPoint end;
	int len = 20;
	int linew = 1;
	int i=0;

	CFOV*cthis =fovOsdCtrl;

	start.x = 367 ;
	start.y = 298 ;
	end.x = start.x;
	end.y = start.y + 5;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	
	start.x = 372 ;
	start.y = 298 ;
	end.x = start.x;
	end.y = start.y + 5;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 367 ;
	start.y = 298 ;
	end.x = start.x+5;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 367 ;
	start.y = 303 ;
	end.x = start.x+5;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	
	return ;
}


void DrawjsAngleFrame(Mat frame,CFOV * fovOsdCtrl,double angle)
{
	Osd_cvPoint start;
	Osd_cvPoint end;
	int len = 20;
	int linew = 1;
	int i=0;

	CFOV*cthis =fovOsdCtrl;
	
	start.x = 665 ;
	start.y = 285 ;
	end.x = start.x+30;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	if(isFovSmall())
	{
		angle = angle/FOVDEGREE_HSMALL*200;
		angle = (angle<200)?angle:200;
		angle = (angle>-200)?angle:-200;
	}
	else
	{
		angle = angle/FOVDEGREE_HLARGE*200;
		angle = (angle<200)?angle:200;
		angle = (angle>-200)?angle:-200;
	}

	start.x = 670 ;
	start.y = 285-angle + 5 ;
	end.x = start.x+25;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 670 ;
	start.y = 285-angle - 5 ;
	end.x = start.x+25;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 670 ;
	start.y = 285-angle - 5 ;
	end.x = start.x;
	end.y = start.y +10;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 670 +25 ;
	start.y = 285-angle - 5 ;
	end.x = start.x;
	end.y = start.y +10;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	return;
}


void Drawjs_leftFrame(Mat frame,CFOV * fovOsdCtrl)
{
	Osd_cvPoint start;
	Osd_cvPoint end;
	int len = 20;
	int linew = 1;
	int i=0;

	for(i=0;i<10;i++)
	{
		start.x = 10 ;
		start.y = 220 + i*len;
		end.x = start.x;
		end.y = start.y +len/2;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	{
		start.x = 10 ;
		start.y = 410;
		end.x = start.x + 6;
		end.y = start.y;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	{
		start.x = 20;
		start.y = 220;
		end.x = start.x -6;
		end.y = start.y;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	for(i=0;i<10;i++)
	{
		start.x = 20 ;
		start.y = 220 + i*len;
		end.x = start.x;
		end.y = start.y +len/2;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	
}


void Drawjs_rightFrame(Mat frame,CFOV * fovOsdCtrl)
{
	Osd_cvPoint start;
	Osd_cvPoint end;
	int len = 20;
	int linew = 1;
	int i=0;

	for(i=0;i<10;i++)
	{
		start.x = 690 ;
		start.y = 220 + i*len;
		end.x = start.x;
		end.y = start.y +len/2;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	{
		start.x = 690 ;
		start.y = 410;
		end.x = start.x + 6;
		end.y = start.y;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	{
		start.x = 700;
		start.y = 220;
		end.x = start.x -6;
		end.y = start.y;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	for(i=0;i<10;i++)
	{
		start.x = 700 ;
		start.y = 220 + i*len;
		end.x = start.x;
		end.y = start.y +len/2;
		DrawcvLine(frame,&start,&end, 2,linew);
	}

	
}


void Drawjs_bottomFrame(Mat frame,CFOV * fovOsdCtrl)
{
	Osd_cvPoint start;
	Osd_cvPoint end;
	int len = 20;
	int linew = 1;
	int i=0;

	CFOV* cthis = fovOsdCtrl;

	for(i=0;i<10;i++)
	{
		start.x = 230 + i*len;
		start.y = 555 ;
		end.x = start.x + len/2;
		end.y = start.y ;
		DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	}

	{
		start.x = 230 ;
		start.y = 555;
		end.x = start.x ;
		end.y = start.y + 6;
		DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	}

	{
		start.x = 420;
		start.y = 565;
		end.x = start.x ;
		end.y = start.y -6;
		DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	}

	for(i=0;i<10;i++)
	{
		start.x = 230 + i*len;
		start.y = 565 ;
		end.x = start.x + len/2;
		end.y = start.y ;
		DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	}

	
}


void DrawjsAlertFrame(Mat frame,CFOV * fovOsdCtrl)
{
	Osd_cvPoint start;
	Osd_cvPoint end;
	int len = 20;
	int linew = 1;
	int i=0;

	CFOV* cthis = fovOsdCtrl;

	/*******************V*******************/
	start.x = 70 ;
	start.y = 80 ;
	end.x = start.x;
	end.y = start.y + 70;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	
	start.x = 70 ;
	start.y = 390 ;
	end.x = start.x;
	end.y = start.y + 70;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	

	start.x = 660 ;
	start.y = 80 ;
	end.x = start.x;
	end.y = start.y + 70;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 660 ;
	start.y = 390 ;
	end.x = start.x;
	end.y = start.y + 70;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	
	/*******************H*******************/
	start.x = 70 ;
	start.y = 80 ;
	end.x = start.x + 100;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	
	start.x = 70 ;
	start.y = 460 ;
	end.x = start.x + 100;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 560 ;
	start.y = 80 ;
	end.x = start.x + 100;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);

	start.x = 560;
	start.y = 460;
	end.x = start.x + 100;
	end.y = start.y;
	DrawcvLine(frame,&start,&end,cthis->frcolor,linew);
	
	return;
}

void Drawcvcross(Mat frame,Line_Param_fb *lineparm)
{

	int centx, centy, width;
	int crossw=width;
	Point pt1,pt2,center;
	if(lineparm==NULL)
	{
		return ;
	}
	Osd_cvPoint start;
	Osd_cvPoint end;
	start.x=lineparm->x-lineparm->width/2;
	start.y=lineparm->y;
	end.x=lineparm->x+lineparm->width/2;
	end.y=lineparm->y;
	DrawcvLine(frame,&start,&end,lineparm->frcolor,1);

	start.x=lineparm->x;
	start.y=lineparm->y-lineparm->width/2;
	end.x=lineparm->x;
	end.y=lineparm->y+lineparm->width/2;
	DrawcvLine(frame,&start,&end,lineparm->frcolor,1);

}

void drawcvrect(Mat frame,int x,int y,int width,int height,int frcolor)
{
	
	CvScalar colour=GetcvColour(frcolor);
	rectangle( frame,Point( x,y ),Point( x+width, y+height),colour, 1, 8);
								

}


void Drawcvrect(Mat frame,Line_Param_fb *lineparm)
{

	int centx, centy;

	Point pt1,pt2,center;
	if(lineparm==NULL)
		{
			return ;
		}
	int x=lineparm->x-lineparm->width/2;
	int y=lineparm->y-lineparm->width/2;
	int width=lineparm->width;
	int height=lineparm->width;

	drawcvrect(frame,x,y,width,height,lineparm->frcolor);
	

}



void Drawcvcrossnum(Mat frame,Line_Param_fb *lineparm)
{

	int centx, centy, width;
	int crossw=width;
	Point pt1,pt2,center;
	if(lineparm==NULL)
		{
			return ;
		}
	char numbuf[10];
	Osd_cvPoint start;
	Osd_cvPoint end;
	int num=lineparm->res0;
	start.x=lineparm->x-lineparm->width/2;
	start.y=lineparm->y;
	end.x=lineparm->x+lineparm->width/2;
	end.y=lineparm->y;
	DrawcvLine(frame,&start,&end,lineparm->frcolor,1);

	start.x=lineparm->x;
	start.y=lineparm->y-lineparm->width/2;
	end.x=lineparm->x;
	end.y=lineparm->y+lineparm->width/2;
	DrawcvLine(frame,&start,&end,lineparm->frcolor,1);
	CvScalar colour=GetcvColour(lineparm->frcolor);
	sprintf(numbuf,"%d",num);
	putText(frame,numbuf,cvPoint(lineparm->x+lineparm->width/2,lineparm->y+lineparm->width/2),CV_FONT_HERSHEY_SIMPLEX,1,colour);

	

}



void DrawcvDashsec(Mat frame,Line_Param_fb *lineparm)
{

	int centx, centy, width;
	int crossw=width;
	Point pt1,pt2,center;
	int startx, starty,endx,endy;
	if(lineparm==NULL)
		{
			return ;
		}
	char numbuf[10];

	DrawcvDashcross(frame,lineparm,2,2);
	startx=lineparm->res0;
	starty=lineparm->res1; 
	endx=lineparm->x;
	endy=lineparm->y;
	
	//DrawcvDashline(frame,lineparm,4,4);
	DrawcvDashliner(frame,startx,starty,endx,endy,4,4,lineparm->frcolor);
	
	

	

}


void Draw_cvgraph_osd(Mat frame,void *lParam)
{


		
		Line_Param_fb * lineParam = (Line_Param_fb *)lParam;
	

		switch(lineParam->objType)
		{
		case grpx_ObjId_Cross:
			Drawcvcross(frame, lineParam);
			break;
		case grpx_ObjId_Rect:
			Drawcvrect(frame, lineParam);
			break;
		case grpx_ObjId_Sectrk:
			DrawcvDashsec(frame, lineParam);
			break;
		case grpx_ObjId_Cross_num:
			Drawcvcrossnum(frame, lineParam);
			break;


		default:
			break;
		}

}


void process_cvosd(Mat frame,void *pPrm)
{
	int devId=0;

	Multich_graphic *grpxChWinPrms=(Multich_graphic *)pPrm;
	int winId;
	Line_Param_fb * lineParam = NULL;
	if(grpxChWinPrms==NULL)
		{
			return;
		}
	for(winId = 0; winId < grpxChWinPrms->chParam[devId].numWindows; winId++)
	{
		lineParam = (Line_Param_fb *)&grpxChWinPrms->chParam[devId].winPrms[winId];
		if(!lineParam->enableWin)
			continue;
		Draw_cvgraph_osd(frame,lineParam);
		
	}

}




#endif
