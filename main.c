#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
#include "image.h"
#include "deque.h"

void applySobelFilter (unsigned char *image, int width, int height) {
    int gx[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };
    int gy[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };
    unsigned char *result = malloc(width * height * 4 * sizeof(unsigned char));
    for (int y = 1; y < height-1; y++) {
        for (int x = 1; x < width-1; x++) {
            int sumX = 0, sumY = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int index = ((y+dy) * width + (x+dx)) * 4;
                    int gray = (image[index] + image[index+1] + image [index+2]) / 3;
                    sumX += gx[dy+1][dx+1] * gray;
                    sumY += gy[dy+1][dx+1] * gray;
                }
            }
            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            if (magnitude > 255) magnitude = 255;
            if (magnitude < 0) magnitude = 0;
            int resultIndex = (y * width + x) * 4;
            result[resultIndex] = (unsigned char) magnitude;
            result[resultIndex+1] = (unsigned char) magnitude;
            result[resultIndex+2] = (unsigned char) magnitude;
            result[resultIndex+3] = image[resultIndex+3];
        }
    }

    for (int i = 0; i < width * height * 4; i++) {
        image[i] = result[i];
    }

    free(result);
}
///------------------------------------------------------------DSU----
void make_set (int v, int* parent, int* rank) {
	parent[v] = v;
	rank[v] = 0;
}

int find_set (int v, int* parent, int* rank) {
	if (v == parent[v])
		return v;
	return parent[v] = find_set(parent[v], parent, rank);
}

void swap(int *a, int *b) {
    int t;
    t = *a, *a = *b, *b = t;
}

void union_sets (int a, int b, int* parent, int* rank) {
	a = find_set(a, parent, rank);
	b = find_set(b, parent, rank);
	if (a != b) {
		if (rank[a] < rank[b])
			swap(&a, &b);
		parent[b] = a;
		if (rank[a] == rank[b])
			rank[a]++;
	}
}
///-------------------------------------------------------------------

double get_delta_rgb(int k, int g, unsigned char *p)
{
    double ans = 0;
    for (int i = 0; i < 3; i++)
    {
        ans += (p[k+i]-p[g+i])*(p[k+i]-p[g+i]);
    }
    return sqrt(ans);
}

double get_delta_bw(int k, int g, unsigned char *p)
{
    return abs(p[k+1]-p[g+1]);
}

double get_delta_bw3(int k, int g, unsigned char *p)
{
    return abs(p[k]-p[g]);
}

double get_delta_bw2(int k, int g, unsigned char *p)
{
    if (p[g]+240 < p[k]) return 0;
    return 100;
}

void f(double max_delta, int h, int w, int n, int x, int* parent, int* rank, unsigned char *picture,
        double (*get_delta)(int, int, unsigned char*))
{
    int ih = 0;
    int jw = 0;
    printf("\nmaxdelta=%lf\n", max_delta);
    printf("%d %d\n", 0, w*h*4);
    for (int k = 0; k < h * w * 4; k += 4)
    {
        for (int i = ih-x; i < ih+x; i++)
        {
            for (int j = jw-x; j < jw+x; j++)
            {
                if (i < 0 || i >= h || j < 0 || j >= w) continue;
                //double delta = get_delta(find_set(k/4, parent, rank)*4, (i*w+j)*4, picture);
                double delta = get_delta(k, (i*w+j)*4, picture);
                //delta *= sqrt((i-ih)*(i-ih) + (j-jw)*(j-jw));
                if (delta <= max_delta) union_sets(k/4, w*i+j, parent, rank);
                //if (k%100000 == 0) printf("%lf ", delta);
            }
        }

        jw++;
        if (jw == w)
        {
            jw = 0;
            ih++;
        }
    }
    for (int k = 0; k < h * w; k += 1)
    {
        parent[k] = find_set(k, parent, rank);
    }
}

void bfs(int x, int d, int a, int b, double max_delta, int h, int w, int* parent, int* was, unsigned char *picture,
         double (*get_delta)(int, int, unsigned char*))
{
    deque q;
    q.length = 0;
    q.first = NULL;
    q.last = NULL;
    push_back(x, &q);
    int k = 0;
    int xii = x%w;
    int ss = 1e8;
//    int war = 1;
//    printf("st__ ");
    while (!empty(&q) && size(&q) < ss)
    {
        int y = front(&q)->x;
        pop_front(&q);
        if (was[y] > 2-b) continue;
        was[y]++;
        int i = y/w;
        int j = y%w;

        int d1=0, d2 = d, d3 = 0, d4 = d;
        if (b < 0) {d3 = -d; d4 = 0;}
        if (a < 0) {d1 = -d; d2 = 0;}

        for (int ii = i+d1; ii < i+d2; ii += 1)
        {
            for (int jj = j+d3; jj < j+d4; jj += 1)
            {
//                int kk = 1e7;
//                if (k % kk == 0) printf("%d: %d, %d || ", k, ii, jj);
//                if (k > kk) return;
                if (ii >= 0 && ii < h && jj >= 0 && jj < w)
                {
                    double delta = get_delta(x*4, (ii*w+jj)*4, picture);
                    if(delta <= max_delta && (parent[ii*w+jj] == ii*w+jj || (ii == xii && parent[ii*w+jj] == x)))
                    {
//                        if (ii*w+jj == x) war = 0;
//                        if (k > kk) printf("%d: %d, %d ", k, ii, jj);
                        parent[ii*w+jj] = x;
                        push_back(ii*w+jj, &q);
                    }
                }
//                if (k > kk) printf("ok\n");
            }
        }
        k++;
    }
//    printf("st222__ ");
}

void f2(double max_delta, int h, int w, int x, int* parent, int* was, unsigned char *picture,
        double (*get_delta)(int, int, unsigned char*))
{

    printf("\nf2: maxdelta=%lf\n", max_delta);
    printf("%d %d\n", 0, w*h*4);
//    int k = 406296;
//    bfs(k, x, 1, 1, max_delta, h, w, parent, was, picture, get_delta);
//    bfs(k, x, 1, -1, max_delta, h, w, parent, was, picture, get_delta);
    int cnt = 0;
    for (int k = 0; k < h * w; k += 1)
    {
        if (parent[k] == k)
        {
            bfs(k, x, 1, 1, max_delta, h, w, parent, was, picture, get_delta);
            bfs(k, x, 1, -1, max_delta, h, w, parent, was, picture, get_delta);

            bfs(k, x, -1, 1, max_delta, h, w, parent, was, picture, get_delta);
            bfs(k, x, -1, -1, max_delta, h, w, parent, was, picture, get_delta);

            cnt++;
        }
    }
}

int main() {

	unsigned int w = 0, h = 0;

	char *filename_input = "images/hand.png";
	unsigned char *picture = load_png_file(filename_input, &w, &h);
	unsigned char *picture2 = load_png_file(filename_input, &w, &h);
	unsigned char *picture3 = load_png_file(filename_input, &w, &h);
	unsigned char *picture4 = load_png_file(filename_input, &w, &h);

	char *filename_output = "images/hand_out13.png";

	if (picture == NULL) {
		printf("I can't read the picture %s. Error.\n", filename_input);
		return -1;
	}

	for (int i = 0; i < h * w * 4; i += 4) {
		//This is an example how to operate with pixel
		//encoded as RGBa / 4 bytes
		struct pixel P;
		P.R = picture[i+0];
		P.G = picture[i+1];
		P.B = picture[i+2];
		P.alpha = picture[i+3];

		char x = (P.R + P.G + P.B) / 3;
		for (int j = 0; j < 4; j++)
        {
            //picture[i+j] = x;
            picture3[i+j] = 0;
            x++;
        }
	}
    //applySobelFilter(picture, w, h);

    int flag = 1;
    if (flag)
    {
        int x = 3;
        int n = h * w;
        printf("n=%d\n", n);
        printf("h=%d\n", h);
        printf("w=%d\n", w);
        int* parent = (int*) malloc(sizeof(int)*n);
        int* rank = (int*) malloc(sizeof(int)*n);
        int* count = (int*) malloc(sizeof(int)*n);
///--------
        for (int i = 0; i < n; i++)
        {
            make_set(i, parent, rank);
            count[i] = 0;
        }
        f(6, h, w, n, x, parent, rank, picture, get_delta_bw2); ///_________________________________1
        for (int k = 0; k < h * w; k += 1)
        {
            count[parent[k]]++;
        }
        for (int k = 0; k < h * w; k += 1)
        {
            if (count[parent[k]] > 1000)
            {
                for (int i = 0; i < 3; i++)
                {
                    picture[4*k+i] = 0;
                }
            }
        }

        applySobelFilter(picture, w, h);
///--------
        for (int i = 0; i < n; i++)
        {
            make_set(i, parent, rank);
            count[i] = 0;
        }
        f(3, h, w, n, 10, parent, rank, picture, get_delta_rgb); ///_________________________________1.5
        for (int k = 0; k < h * w; k += 1)
        {
            int x = 0;
            for (int i = 0; i < 3; i++)
            {
                x += picture[4*parent[k]+i];
            }
            x /= 3;
            for (int i = 0; i < 3; i++)
            {
                picture[4*k+i] = x;
            }
        }
        int ih = 0;
        int jw = 0;
        int x2 = 2;
        for (int k = 0; k < h * w; k += 1)
        {
            if (picture[4*k] > 200)
            {
                for (int i = ih-x2; i < ih+x2; i++)
                {
                    for (int j = jw-x2; j < jw+x2; j++)
                    {
                        if (i < 0 || i >= h || j < 0 || j >= w) continue;
                        for (int ii = 0; ii < 4; ii++)
                            picture3[(i*w+j)*4+ii] = picture[k*4+ii];
                    }
                }
            }

            jw++;
            if (jw == w)
            {
                jw = 0;
                ih++;
            }
        }

        ih = 0;
        jw = 0;
        int x3 = 1;
        for (int k = 0; k < h * w; k += 1)
        {
            if (picture3[4*k] > 200)
            {
                for (int i = ih-x3; i < ih+x3; i++)
                {
                    for (int j = jw-x3; j < jw+x3; j++)
                    {
                        if (i < 0 || i >= h || j < 0 || j >= w) continue;
                        for (int ii = 0; ii < 3; ii++)
                            picture2[4*(i*w+j)+ii] = 255;
                    }
                }
            }

            jw++;
            if (jw == w)
            {
                jw = 0;
                ih++;
            }
        }

        //applySobelFilter(picture2, w, h);

//        for (int i = 0; i < h * w * 4; i += 4)
//        {
//            char x = picture4[i+0] + picture4[i+1] + picture[i+2];
//            x /= 3;
//            picture4[i+0] = x;
//            picture4[i+1] = x;
//            picture4[i+2] = x;
//        }
///--------
        for (int i = 0; i < n; i++)
        {
            make_set(i, parent, rank);
            count[i] = 0;
        }
        f2(48, h, w, 5, parent, rank, picture2, get_delta_bw); ///_________________________________2
        for (int k = 0; k < h * w; k += 1)
        {
            if (parent[k] == k)
            {
                for (int i = 0; i < 3; i++)
                {
                    picture2[4*k+i] = (rand()+140)%256;
                }
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    picture2[4*k+i] = picture2[4*parent[k]+i];
                }
            }
        }
//--------------------------------------------------------------------
        for (int k = 0; k < h * w; k += 1)
        {
            count[parent[k]]++;
        }
        for (int k = 0; k < h * w; k += 1)
        {
//            if (count[parent[k]] > 10)
//                printf("%d\n", count[parent[k]]);
            if (count[parent[k]] > 300000)
            {
                for (int i = 0; i < 3; i++)
                {
                    picture2[4*k+i] = 0;
                }
            }
            if (count[parent[k]] < 1000)
            {
                for (int i = 0; i < 3; i++)
                {
                    picture2[4*k+i] = 0;
                }
            }
        }

//
//        applySobelFilter(picture, w, h);

        free(parent);
        free(rank);
        free(count);
    }

	save_png_file(filename_output, picture2, w, h);
	free(picture);
	free(picture2);
	free(picture3);
	free(picture4);

	return 0;
}

