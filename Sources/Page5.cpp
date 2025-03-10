/*
 Copyright © 2021  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.
    
    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/


#ifndef Page5_H
#define Page5_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NBTWriter.h"
#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "D:/zlib-1.2.11/zlib.h"
#define bufferSize 1024
bool compressFile(const char*sourcePath,const char*destPath)
{
    char buf[bufferSize]={0};
    FILE*in=NULL;
    gzFile out=NULL;
    int len=0;
    fopen_s(&in,sourcePath,"rb");
    out=gzopen(destPath,"wb");
    if(in==NULL||out==NULL)
        return false;
    while(true)
    {
        len=(int)fread(buf,1,sizeof(buf),in);
        if(ferror(in))return false;
        if(len==0)break;
        if(len!=gzwrite(out,buf,(unsigned)len))
            return false;
        memset(buf,0,sizeof(buf));
    }
    fclose(in);
    gzclose(out);
    qDebug("succeed");
    return true;
}

void MainWindow::on_AllowNaturalOpti_stateChanged(int arg1)
{
    Data.allowNaturalOpti=Data.isSurvival()&&ui->AllowNaturalOpti->isChecked();
}

void MainWindow::on_ExportLite_clicked()
{
    if(Data.ExLitestep<2)return;
    if(Data.step<5)return;
    QString FileName=QFileDialog::getSaveFileName(this,tr("导出为投影"),"",tr("投影文件(*.litematic)"));

    if(FileName.isEmpty())return;

    if(!FileName.endsWith(".litematic"))
    {
        qDebug("得到的文件路径有错！");
        return;
    }
    QString unCompressed=FileName.left(FileName.length()-strlen(".litematic"))+".TokiNoBug";

    ui->ShowProgressExLite->setMaximum(100+Data.size3D[0]*Data.size3D[1]*Data.size3D[2]);
    ui->ShowProgressExLite->setValue(0);

    qDebug("开始导出投影");

    Data.exportAsLitematica(unCompressed);

    if(compressFile(unCompressed.toLocal8Bit().data(),FileName.toLocal8Bit().data()))
    {
        ui->ShowProgressExLite->setValue(100+Data.size3D[0]*Data.size3D[1]*Data.size3D[2]);
        qDebug("压缩成功");
        Data.ExLitestep=4;
        QFile umComFile(unCompressed);
        umComFile.remove();
        Data.ProductPath=FileName;
    }
    else
    {
        qDebug("压缩失败");
        return;
    }

    Data.step=6;updateEnables();
    qDebug("导出为投影成功");
    ui->FinishExLite->setEnabled(true);    
    ui->seeExported->setEnabled(true);
    return;

}

void MainWindow::on_Build4Lite_clicked()
{
    ui->ShowProgressExLite->setMaximum(8*Data.sizePic[0]*Data.sizePic[1]);
    ui->ShowProgressExLite->setValue(0);

    ui->ExportLite->setEnabled(false);
    ui->FinishExLite->setEnabled(false);



    Data.ExLitestep=0;

    qDebug("开始makeHeight");
    Data.makeHeight();
    ui->ShowLiteXYZ->setText("X:"+QString::number(Data.size3D[0])+"  × Y:"+QString::number(Data.size3D[1])+"  × Z:"+QString::number(Data.size3D[2]));
    Data.ExLitestep=1;    
    Data.step=5;updateEnables();
    updateEnables();
    if((Data.Height.array()<0).any())
    {
        qDebug("调整高度出错：出现了负高度");
        return;
    }

    qDebug("开始BuildHeight");
    long blockCount=Data.BuildHeight();
    ui->ShowLiteBlocks->setText(QString::number(blockCount));
    Data.ExLitestep=2;

    ui->ShowProgressExLite->setValue(8*Data.sizePic[0]*Data.sizePic[1]);
    ui->ExportLite->setEnabled(true);
}

long mcMap::makeHeight()//整张图片将被遍历4遍（向量化的遍历也被视为遍历）
{
    int u=0;
    qDebug()<<u++;
if(step<5)return 0;
if(ExLitestep<0)return 0;

Base=mapPic/4;
qDebug()<<u++;
Depth.setZero(sizePic[0],sizePic[1]);
qDebug()<<u++;

Depth=mapPic-4*(mapPic/4);

/*for(short r=0;r<sizePic[0];r++)
    for(short c=0;c<sizePic[1];c++)
        Depth(r,c)=mapPic(r,c)%4;*/

qDebug()<<u++;

if((Depth.array()>=3).any())
{
    qDebug()<<"出现错误：有深度为3的元素出现";
    return 0;
}

qDebug()<<u++;

MatrixXi isWater=(Base.array()==12).select(MatrixXi::Ones(sizePic[0],sizePic[1]),0);

qDebug()<<u++;
MatrixXi dealedDepth=(Base.array()!=12).select(Depth,1)-MatrixXi::Ones(sizePic[0],sizePic[1]);
dealedDepth=(Base.array()>0).select(dealedDepth,0);
//dealedDepth为处理后的depth，也是真正意义的高度差。其中水的高度差被重置为0(它们的颜色仅由深度决定，与高度无关)
qDebug()<<u++;

int waterCount=(Base.array()==12).count();
qDebug()<<"共有"<<waterCount<<"个水柱";
//if(waterCount)
WaterList.setZero(4,waterCount);
//类似于稀疏矩阵，每一列代表一个水，第一行是行坐标，第二行是列坐标，第三行是水柱顶点高度，第四行是水柱低点高度
qDebug()<<u++;
Height.setZero(1+sizePic[0],sizePic[1]);

qDebug()<<u++;
int writeWaterIndex=0;

for(short r=0;r<sizePic[0];r++)//遍历每一行，根据高度差构建高度图
{
    Height.row(r+1)=Height.row(r)+dealedDepth.row(r);
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[0]);
}
qDebug()<<u++;


MatrixXi LowMap(sizePic[0]+1,sizePic[1]);
LowMap<<Height;

qDebug()<<u++;
for(short r=0;r<sizePic[0];r++)//抓取WaterList，并为水柱处理HighMap和LowMap
for(short c=0;c<sizePic[1];c++)
{
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+1);

    if(!isWater(r,c)||(Base(r,c)!=12))continue;

    if(writeWaterIndex>=waterCount)
    {
        qDebug("出现错误：writeWaterIndex>=waterCount");
        continue;
    }
    WaterList(0,writeWaterIndex)=r;
    WaterList(1,writeWaterIndex)=c;
    WaterList(2,writeWaterIndex)=Height(r+1,c);

switch (Depth(r,c))
{
case 0:
    WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex);
    break;
case 1:
    WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex)-4;
    LowMap(r+1,c)=WaterList(3,writeWaterIndex);
    break;
case 2:
    WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex)-9;
    LowMap(r+1,c)=WaterList(3,writeWaterIndex);
    break;
default:
    qDebug()<<"出现错误：("<<r<<','<<c<<")处出现了Depth为3的水";
    return 0;
}
Height(r+1,c)++;//遮顶玻璃
writeWaterIndex++;
}

qDebug()<<u++;


for(short c=0;c<sizePic[1];c++)//将每一列沉降。优化可以放在这里
{
    Height.col(c).array()-=LowMap.col(c).minCoeff();
    LowMap.col(c).array()-=LowMap.col(c).minCoeff();//此后不再使用LowMap，所以这一句是多余的（除非使用了无损高度压缩）
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[0]);
}

qDebug()<<u++;
/*if(waterCount)//重新获取WaterList
{
    writeWaterIndex=0;
    for(short r=0;r<sizePic[0];r++)
    for(short c=0;c<sizePic[1];c++)
    {
        parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+1);
        if(!isWater(r,c)||(Base(r,c)!=12))
        {
            continue;
        }
        if(writeWaterIndex>=waterCount)
        {
            qDebug("出现错误：writeWaterIndex>=waterCount");
            continue;
        }
        WaterList(0,writeWaterIndex)=r;
        WaterList(1,writeWaterIndex)=c;
        WaterList(2,writeWaterIndex)=Height(r+1,c)-1;

    switch (Depth(r,c))
    {
    case 0:
        WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex);
        break;
    case 1:
        WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex)-4;
        //LowMap(r+1,c)=WaterList(3,writeWaterIndex);
        break;
    case 2:
        WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex)-9;
        //LowMap(r+1,c)=WaterList(3,writeWaterIndex);
        break;
    default:
        qDebug()<<"出现错误：("<<r<<','<<c<<")处出现了Depth为3的水";
        return 0;

    }
    writeWaterIndex++;
    }
}*/

//qDebug()<<u++;

if(allowNaturalOpti)
{
OptiTree Tree;
VectorXi TempHM,TempLM;
HeightLine::Base=Base;
    for(int c=0;c<sizePic[1];c++)
    {
        TempHM=Height.col(c);
        TempLM=LowMap.col(c);
        HeightLine::currentColum=c;
        Tree.NaturalOpti(TempHM,TempLM);
        Height.col(c)=TempHM;
        LowMap.col(c)=TempLM;
        parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[0]);
    }
    if(waterCount)
    {
        writeWaterIndex=0;
        for(short r=0;r<sizePic[0];r++)
        for(short c=0;c<sizePic[1];c++)
        {
            parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+1);
            if(!isWater(r,c)||(Base(r,c)!=12))
            {
                continue;
            }
            if(writeWaterIndex>=waterCount)
            {
                qDebug("出现错误：writeWaterIndex>=waterCount");
                continue;
            }
            WaterList(0,writeWaterIndex)=r;
            WaterList(1,writeWaterIndex)=c;
            WaterList(2,writeWaterIndex)=Height(r+1,c)-1;

        switch (Depth(r,c))
        {
        case 0:
            WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex);
            break;
        case 1:
            WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex)-4;
            //LowMap(r+1,c)=WaterList(3,writeWaterIndex);
            break;
        case 2:
            WaterList(3,writeWaterIndex)=WaterList(2,writeWaterIndex)-9;
            //LowMap(r+1,c)=WaterList(3,writeWaterIndex);
            break;
        default:
            qDebug()<<"出现错误：("<<r<<','<<c<<")处出现了Depth为3的水";
            return 0;

        }
        writeWaterIndex++;
        }
    }
}
else
parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+2*sizePic[0]*sizePic[1]);


size3D[2]=2+Base.rows();//z
size3D[0]=2+Base.cols();//x
size3D[1]=1+Height.array().maxCoeff();//y


//Base(r,c)<->Depth(r,c)<->Height(r+1,c)

ExLitestep=1;
return (sizePic[0]+1)*sizePic[1];
}

long mcMap::BuildHeight()//进度条上表现为遍历3遍图像
{

    if(step<5)return 0;
    if(ExLitestep<1)return 0;
    Build.resize(size3D[0],size3D[1],size3D[2]);
    qDebug()<<0;
    Build.setZero();
    qDebug()<<1;
    int TotalBlockCount=0;

    //Base(r,c)<->Depth(r,c)<->Height(r+1,c)<->Build(c+1,Height(r+1,c),r+1)
    //为了区分玻璃与空气，张量中存储的是Base+1.所以元素为1对应着玻璃，0对应空气
    int x=0,y=0,z=0;

    int yLow=0;

    int waterCount=WaterList.cols();
    qDebug()<<"共有"<<waterCount<<"个水柱";
    qDebug()<<2;
    if(waterCount)
    for(int waterIndex=0;waterIndex<WaterList.cols();waterIndex++)
    {
        x=WaterList(1,waterIndex)+1;
        z=WaterList(0,waterIndex)+1;
        y=WaterList(2,waterIndex);
        yLow=WaterList(3,waterIndex);

        Build(x,y+1,z)=0+1;//柱顶玻璃

        for(short yDynamic=yLow;yDynamic<=y;yDynamic++)
        {
            Build(x-1,yDynamic,z-0)=1;
            Build(x+1,yDynamic,z+0)=1;
            Build(x+0,yDynamic,z-1)=1;
            Build(x+0,yDynamic,z+1)=1;
            //Build(x,yDynamic,z)=13;
        }
        if(yLow>=1)
            Build(x,yLow-1,z)=1;//柱底玻璃
    }
    qDebug()<<3;
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]*sizePic[0]);

    for(short r=0;r<sizePic[0];r++)
    {
        for(short c=0;c<sizePic[1];c++)
        {
            x=c+1;y=Height(r+1,c);z=r+1;
            if(y>=1&&parent->NeedGlass[Base(r,c)][SelectedBlockList[Base(r,c)]])
                Build(x,y-1,z)=0+1;
            if(Base(r,c)==12||Base(r,c)==0)
                continue;

            Build(x,y,z)=Base(r,c)+1;
        }
        parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]);
    }

    for(short c=0;c<sizePic[1];c++)
        Build(c+1,Height(0,c),0)=11+1;


qDebug()<<4;

    qDebug()<<5;

parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]*sizePic[0]);

if(waterCount)
for(int waterIndex=0;waterIndex<WaterList.cols();waterIndex++)
{
    x=WaterList(1,waterIndex)+1;
    z=WaterList(0,waterIndex)+1;
    y=WaterList(2,waterIndex);
    yLow=WaterList(3,waterIndex);

    Build(x,y+1,z)=0+1;//柱顶玻璃

    for(short yDynamic=yLow;yDynamic<=y;yDynamic++)
    {
        //Build(x-1,yDynamic,z-0)=1;
        //Build(x-1,yDynamic,z+0)=1;
        //Build(x+0,yDynamic,z-1)=1;
        //Build(x+0,yDynamic,z+1)=1;
        Build(x,yDynamic,z)=13;
    }

}
for(x=0;x<size3D[0];x++)
    for(y=0;y<size3D[1];y++)
        for(z=0;z<size3D[2];z++)
            if(Build(x,y,z)) TotalBlockCount++;
totalBlocks=TotalBlockCount;

    return TotalBlockCount;
}



long mcMap::exportAsLitematica(QString FilePathAndName)
{
    NBT::NBTWriter Lite;

    Lite.open(FilePathAndName.toLocal8Bit().data());
    QString LiteName=parent->ui->InputLiteName->toPlainText();
    QString author=parent->ui->InputAuthor->toPlainText();
    QString RegionName=parent->ui->InputRegionName->toPlainText();

    Lite.writeCompound("Metadata");
        Lite.writeCompound("EnclosingSize");
            Lite.writeInt("x",size3D[0]);
            Lite.writeInt("y",size3D[1]);
            Lite.writeInt("z",size3D[2]);
        Lite.endCompound();
        Lite.writeString("Author",author.toUtf8().data());
        Lite.writeString("Description","This litematic is generated by SlopeCraft, developer TokiNoBug");
        Lite.writeString("Name",LiteName.toUtf8().data());
        Lite.writeInt("RegionCount",1);
        Lite.writeLong("TimeCreated",114514);
        Lite.writeLong("TimeModified",1919810);
        Lite.writeInt("TotalBlocks",totalBlocks);
        Lite.writeInt("TotalVolume",size3D[0]*size3D[1]*size3D[2]);
    Lite.endCompound();
parent->ui->ShowProgressExLite->setValue(50);
    Lite.writeCompound("Regions");
        Lite.writeCompound(RegionName.toUtf8().data());
            Lite.writeCompound("Position");
                Lite.writeInt("x",0);
                Lite.writeInt("y",0);
                Lite.writeInt("z",0);
            Lite.endCompound();
            Lite.writeCompound("Size");
                Lite.writeInt("x",size3D[0]);
                Lite.writeInt("y",size3D[1]);
                Lite.writeInt("z",size3D[2]);
            Lite.endCompound();
            parent->ui->ShowProgressExLite->setValue(100);
            Lite.writeListHead("BlockStatePalette",NBT::idCompound,131);
                {
                    short written=(is16()?59:52);
                    if(is17())written=61;
                    vector<QString> ProName,ProVal;
                    //bool isNetBlockId;
                    QString netBlockId;

                    dealBlockId("air",netBlockId,ProName,ProVal);
                    writeBlock(netBlockId,ProName,ProVal,Lite);
                    for(short r=0;r<written;r++)
                    {
                        dealBlockId(BlockListId[r],netBlockId,ProName,ProVal);
                        writeBlock(netBlockId,ProName,ProVal,Lite);
                    }//到此写入了written+1个方块，还需要写入130-written个

                    writeTrash(130-written,Lite);
                }
            Lite.writeListHead("Entities",NBT::idCompound,0);
            Lite.writeListHead("PendingBlockTicks",NBT::idCompound,0);
            Lite.writeListHead("PendingFluidTiccks",NBT::idCompound,0);
            Lite.writeListHead("TileEntities",NBT::idCompound,0);
            {
                int ArraySize;
                //Lite.writeLong("aLong",1145141919810);
                int Volume=size3D[0]*size3D[1]*size3D[2];
                ArraySize=((Volume%8)?(Volume/8+1):Volume/8);
                long long HackyVal=sizeof(long long);
                char *inverser=(char*)&HackyVal;
                short inverserIndex=7;
            Lite.writeLongArrayHead("BlockStates",ArraySize);
            for(int y=0;y<size3D[1];y++)
                for(int z=0;z<size3D[2];z++)
                {
                    for(int x=0;x<size3D[0];x++)
                    {
                        inverser[inverserIndex--]=Build(x,y,z);

                        if(inverserIndex<0)
                        {

                            inverserIndex=7;
                            Lite.writeLongDirectly("id",HackyVal);
                        }
                    }
                    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+size3D[0]);
                }

            if(!Lite.isListFinished())
                Lite.writeLongDirectly("id",HackyVal);
            }
    Lite.endCompound();
Lite.endCompound();
switch (gameVersion)
{
case 12:
    Lite.writeInt("MinecraftDataVersion",1343);
    Lite.writeInt("Version",4);
    break;
case 13:
    Lite.writeInt("MinecraftDataVersion",1631);
    Lite.writeInt("Version",5);
    break;
case 14:
    Lite.writeInt("MinecraftDataVersion",1976);
    Lite.writeInt("Version",5);
    break;
case 15:
    Lite.writeInt("MinecraftDataVersion",2230);
    Lite.writeInt("Version",5);
    break;
case 16:
    Lite.writeInt("MinecraftDataVersion",2586);
    Lite.writeInt("Version",5);
    break;
case 17:
    Lite.writeInt("MinecraftDataVersion",2724);
    Lite.writeInt("Version",5);
    break;
default:
    qDebug("错误的游戏版本！");break;
}



Lite.close();
    return Lite.getByteCount();
}

#endif
