#include"osd_cv.h"


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
