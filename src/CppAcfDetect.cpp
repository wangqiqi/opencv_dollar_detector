//esse código fonte serve como o main do projeto

#include "CppAcfDetect.h"

int main(int argc, char** argv) 
{
	Mat image;
	int lastFrame = 1;
	int firstFrame = 1;
	int step = 1;
	stringstream fileName;
	stringstream windowName;
	Detector d;

    QApplication a(argc, argv);
    /*MainWindow w;
    w.show();*/

    //Chamada para a leitura do detector a ser usado
    d.readDetectorModel("/home/gfuhr/Dropbox/Fuhr_Charles/QtAcfDetect/QtAcfDetect/detector.xml");

    //Loop para analisar os frames
	for (int i = firstFrame; i <= lastFrame; i = i + step)
	{
        /*fileName << PREFIX << i << EXTENSION;
		windowName << "Frame #" << i;
		//image = imread(filename.str(), CV_LOAD_IMAGE_UNCHANGED);
		image = imread("D:\\Workspace\\CppAcfDetect\\CppAcfDetect\\img.jpg", CV_LOAD_IMAGE_UNCHANGED);
		if (image.empty()) //check if the image was loaded
			return -1;
        imshow(windowName.str(), image);*/
	}

    //waitKey(0);
	destroyAllWindows();
    return a.exec();
}

//Esboço para a função de detecção aplicada a uma imagem
//Está colocada aqui, mas pode ser na classe Detector também
BoundingBox* acfDetectImg(Mat image, Detector detector)
{
    BoundingBox bb;
    int detectorLength = 100;

    //teste para ver se o conteudo da imagem eh char, se for aplica a funcao imreadf

    //calcular a piramide da imagem

    //criar uma matriz de bounding boxes bbs[nmero de escalas da pirmida][nmero de elementos do detector]

    //loop copiado do código original
    for (int i = 0; i < detector.opts.pPyramid.nScales; i++)
    {
        for (int j = 0; j < detectorLength; j++)
        {
            //chns  o primeiro elemento de prhr
            float* chns = NULL;
            //trees  um vetor lido do segundo elemento de prhs
            Mat *trees = NULL;
            const int shrink = detector.opts.pPyramid.pChns.shrink;
            //modelHt  o quarto elemento de prhs
            const int modelHt = 50;
            //modelWd  o quinto elemento de prhs
            const int modelWd = 50;
            //stride  o sexto elemento de prhs
            const int stride = 2;
            //cascThr  lido como prhs[6]
            const float cascThr = 1.0;

            //extrair informaes dos campos de trees
            float *thrs = NULL;
            float *hs = NULL;
            uint32_t *fids = NULL;
            uint32_t *child = NULL;
            const int treeDepth = 0;

            //chnsSize  a quantidade de elementos em cada dimenso de prhs[0]
            const int chnsSize0 = 3, chnsSize1 = 3;//dummy
            const int height = chnsSize0;
            const int width = chnsSize1;
            const int nChns = 0;
            const int *fidsSize = NULL;
            //nTreeNodes  a quantidade de elementos na primeira dimenso de fids
            const int nTreeNodes = 0;
            //nTrees  a quantidade de elementos na segunda dimenso de fids
            const int nTrees = 0;
            const int height1 = (int)ceil(float(height*shrink - modelHt + 1 / stride));
            const int width1 = (int)ceil(float(width*shrink - modelWd + 1 / stride));

            //construct cids array
            int nFtrs = modelHt / shrink*modelWd / shrink*detector.info.colorCh.nChannels;
            uint32_t *cids = new uint32_t[nFtrs];
            int m = 0;
            for (int z = 0; z<detector.info.colorCh.nChannels; z++)
            for (int c = 0; c<modelWd / shrink; c++)
            for (int r = 0; r<modelHt / shrink; r++)
                cids[m++] = z*width*height + c*height + r;


            // apply classifier to each patch
            vector<int> rs, cs; vector<float> hs1;
            for (int c = 0; c<width1; c++) for (int r = 0; r<height1; r++) {
                float h = 0, *chns1 = chns + (r*stride / shrink) + (c*stride / shrink)*height;
                if (treeDepth == 1) {
                    // specialized case for treeDepth==1
                    for (int t = 0; t < nTrees; t++) {
                        uint32_t offset = t*nTreeNodes, k = offset, k0 = 0;
                        detector.getChild(chns1, cids, fids, thrs, offset, k0, k);
                        h += hs[k]; if (h <= cascThr) break;
                    }
                }
                else if (treeDepth == 2) {
                    // specialized case for treeDepth==2
                    for (int t = 0; t < nTrees; t++) {
                        uint32_t offset = t*nTreeNodes, k = offset, k0 = 0;
                        detector.getChild(chns1, cids, fids, thrs, offset, k0, k);
                        detector.getChild(chns1, cids, fids, thrs, offset, k0, k);
                        h += hs[k]; if (h <= cascThr) break;
                    }
                }
                else if (treeDepth>2) {
                    // specialized case for treeDepth>2
                    for (int t = 0; t < nTrees; t++) {
                        uint32_t offset = t*nTreeNodes, k = offset, k0 = 0;
                        for (int i = 0; i<treeDepth; i++)
                            detector.getChild(chns1, cids, fids, thrs, offset, k0, k);
                        h += hs[k]; if (h <= cascThr) break;
                    }
                }
                else {
                    // general case (variable tree depth)
                    for (int t = 0; t < nTrees; t++) {
                        uint32_t offset = t*nTreeNodes, k = offset, k0 = k;
                        while (child[k]) {
                            float ftr = chns1[cids[fids[k]]];
                            k = (ftr<thrs[k]) ? 1 : 0;
                            k0 = k = child[k0] - k + offset;
                        }
                        h += hs[k]; if (h <= cascThr) break;
                    }
                }
                if (h>cascThr) { cs.push_back(c); rs.push_back(r); hs1.push_back(h); }
            }
        }
    }
    return NULL;
}
