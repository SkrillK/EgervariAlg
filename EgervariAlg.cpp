#include <iostream>
#include <vector>
#include <iomanip>

#include <fstream>
#include <string>
#include <sstream>


using namespace std;

struct ChainVertex
{
	int i;
	int j;
	int value;
};

vector<vector<int>> matrixRead()
{
	ifstream file;

	vector<vector<int>> matrix;

	file.open("matrix.txt");
	if (file.is_open())
	{
		vector<int> row;
		string s1, s2;

		while (getline(file, s1, '\n'))
		{
			stringstream line(s1);

			while (getline(line, s2, ' '))
				row.push_back(stoi(s2));

			matrix.push_back(row);
			row.clear();
		}
	}

	return matrix;
}

void matrixPrint(vector<vector<int>>& matrix, int n)
{
	for (int i = 0; i < matrix.size(); i++)
	{
		cout << setw(n + 1);
		for (int j = 0; j < matrix[0].size(); j++)
			cout << matrix[i][j] << setw(3);
		cout << "\n";
	}
	cout << "\n";
}

//---------------------------------------------------------------

int maxElFind(vector<vector<int>>& matrix)
{
	int maxEl = 0;

	for (int i = 0; i < matrix.size(); i++)
		for (int j = 0; j < matrix[0].size(); j++)
			if (matrix[i][j] > maxEl)
				maxEl = matrix[i][j];

	return maxEl;
}

void inverseMatrix(vector<vector<int>>& matrix)
{
	int maxNum = maxElFind(matrix);

	for (int i = 0; i < matrix.size(); i++)
		for (int j = 0; j < matrix[0].size(); j++)
			matrix[i][j] = maxNum - matrix[i][j];
}

void reduction(vector<vector<int>>& matrix)
{
	int minEl;

	for (int i = 0; i < matrix.size(); i++)
	{
		minEl = INT_MAX;

		for (int j = 0; j < matrix[0].size(); j++)
			if (matrix[i][j] < minEl)
				minEl = matrix[i][j];

		if (minEl == 0)
			continue;

		for (int j = 0; j < matrix[0].size(); j++)
			matrix[i][j] = matrix[i][j] - minEl;
	}

	for (int j = 0; j < matrix[0].size(); j++)
	{
		minEl = INT_MAX;

		for (int i = 0; i < matrix.size(); i++)
			if (matrix[i][j] < minEl)
				minEl = matrix[i][j];

		if (minEl == 0)
			continue;

		for (int i = 0; i < matrix.size(); i++)
			matrix[i][j] = matrix[i][j] - minEl;
	}
}

vector<vector<int>> condidatesMatrixFind(vector<vector<int>>& matrix)
{
	vector<vector<int>> condMatrix = matrix;

	for (int i = 0; i < matrix.size(); i++)
		for (int j = 0; j < matrix[0].size(); j++)
			if (condMatrix[i][j] > 0)
				condMatrix[i][j] = 0;
			else
				condMatrix[i][j] = 1;

	return condMatrix;
}

vector<vector<int>> selectedCondidatesMatrixFind(vector<vector<int>>& condMatrix, vector<int>& selected)
{
	vector<vector<int>> sCondMatrix = condMatrix;
	bool flagCol, flagRow;

	for (int i = 0; i < condMatrix.size(); i++)
	{
		flagRow = false;

		for (int j = 0; j < condMatrix[0].size(); j++)
		{
			flagCol = false;

			for (int k = 0; k < i; k++)
				if (sCondMatrix[k][j] == 1)
					flagCol = true;

			if (sCondMatrix[i][j] == 1)
			{
				if (flagCol || flagRow)
					sCondMatrix[i][j] = -1;
				else
					flagRow = true;
			}

		}
	}

	for (int i = 0; i < condMatrix.size(); i++)
		for (int j = 0; j < condMatrix[0].size(); j++)
			if (sCondMatrix[i][j] == 1)
			{
				selected[i] = j;
				continue;
			}

	return sCondMatrix;
}

//---------------------------------------------------------------

void egervariReduction(vector<vector<int>>& matrix, vector<int>& notSelected_i, vector<int>& notSelected_j)
{
	int minAlpha = INT_MAX;

	for (int i = 0; i < notSelected_i.size(); i++)
		for (int j = 0; j < matrix[0].size(); j++)
		{
			int num = matrix[notSelected_i[i]][j];

			if (num < minAlpha && num != 0)
				minAlpha = num;
		}

	for (int j = 0; j < notSelected_j.size(); j++)
		for (int i = 0; i < matrix.size(); i++)
		{
			int num = matrix[i][notSelected_j[j]];

			if (num < minAlpha && num != 0)
				minAlpha = num;
		}

	//--------------

	for (int i = 0; i < notSelected_i.size(); i++)
		for (int j = 0; j < matrix[0].size(); j++)
			matrix[notSelected_i[i]][j] -= minAlpha;


	for (int j = 0; j < notSelected_j.size(); j++)
		for (int i = 0; i < matrix.size(); i++)
			matrix[i][notSelected_j[j]] += minAlpha;
}

vector<ChainVertex> chainCreate(vector<vector<int>>& matrixB, vector<ChainVertex>& chain, vector<vector<int>>& chainMatrix, int& chainSize, int& numOfActions)
{
	ChainVertex nextVertex;

	if (chain[0].value == -1)
	{
		for (int i = 0; i < matrixB.size(); i++)
			if (matrixB[i][chain[0].j] == 1)
			{
				if (chainMatrix[chain[0].i][chain[0].j] > 0)
					if (i + chainMatrix[chain[0].i][chain[0].j] - 1 >= chainMatrix.size())
					{
						if (chain.size() % 2 == 1)
							return chain;
						else
						{
							chain.erase(chain.begin());
							return chain;
						}
					}
					else
						i += (chainMatrix[chain[0].i][chain[0].j] - 1);

				if (chainMatrix[i][chain[0].j] > 0)
				{
					ChainVertex cycleVertex;
					cycleVertex.i = i;
					cycleVertex.j = chain[0].j;

					for (int k = 0; k < chain.size(); k++)
						if (chain[0].i != cycleVertex.i || chain[0].j != cycleVertex.j) 
						{
							chain.erase(chain.begin());
							chainSize--;
							numOfActions++;
						}
						else
							break;
				}
				else
				{
					nextVertex.i = i;
					nextVertex.j = chain[0].j;
					nextVertex.value = 1;

					chain.insert(chain.begin(), nextVertex);
					chainSize++;
					numOfActions++;
				}

				chainMatrix[i][chain[0].j]++;

				chainCreate(matrixB, chain, chainMatrix, chainSize, numOfActions);

				break;
			}
	}
	else
	{
		for (int j = 0; j < matrixB[0].size(); j++)
			if (matrixB[chain[0].i][j] == -1)
			{
				if (chainMatrix[chain[0].i][chain[0].j] > 0)
					if (j + chainMatrix[chain[0].i][chain[0].j] - 1 >= chainMatrix[0].size())
					{
						if (chain.size() % 2 == 1)
							return chain;
						else
						{
							chain.erase(chain.begin());
							return chain;
						}
					}
					else
						j += (chainMatrix[chain[0].i][chain[0].j] - 1);

				if (chainMatrix[chain[0].i][j] > 0)
				{
					ChainVertex cycleVertex;
					cycleVertex.i = chain[0].i;
					cycleVertex.j = j;

					for (int k = 0; k < chain.size(); k++)
						if (chain[0].i != cycleVertex.i || chain[0].j != cycleVertex.j)
						{
							chain.erase(chain.begin());
							chainSize--;
							numOfActions++;
						}
						else
							break;
				}
				else
				{
					nextVertex.i = chain[0].i;
					nextVertex.j = j;
					nextVertex.value = -1;

					chain.insert(chain.begin(), nextVertex);
					chainSize++;
					numOfActions++;
				}

				chainMatrix[chain[0].i][j]++;

				chainCreate(matrixB, chain, chainMatrix, chainSize, numOfActions);

				break;
			}
	}

	return chain;
}

bool chainCheck(vector<vector<int>>& matrixB, ChainVertex& startVertex, int num)
{
	vector<vector<int>> chainMatrix(matrixB.size(), vector<int>(matrixB[0].size(), 0));

	int chainSize = 1, numOfActions = 0;
	vector<ChainVertex> chain;
	chain.push_back(startVertex);

	chainCreate(matrixB, chain, chainMatrix, chainSize, numOfActions);

	if (chain.size() % 2 == 1)
	{
		for (int i = 0; i < chain.size(); i++)
			chain[i].value = -chain[i].value;


		for (int i = 0; i < chain.size(); i++)
		{
			matrixB[chain[i].i][chain[i].j] = chain[i].value;
		}

		cout << "== Result ========================\n";
		cout << "   Matrix B`" << num + 1 << ":\n";
		matrixPrint(matrixB, 3);

		cout << "   Chain inverted.\n";
		cout << "   Size of chain: " << chainSize << "\n";
		cout << "   Number of actions: " << numOfActions << "\n";

		return true;
	}

	cout << "   Chain not inverted.\n";
	cout << "   Size of chain: " << chainSize << "\n";
	cout << "   Number of actions: " << numOfActions << "\n\n";

	return false;
}

void egervariAlg(vector<vector<int>>& matrix, vector<vector<int>>& matrixB, vector<int>& selected, int num, int& retryNum)
{
	vector<int> notSelected_i, notSelected_j;
	bool flag;

	for (int k = 0; k < selected.size(); k++)
		if (selected[k] == -1)
		{
			for (int j = 0; j < matrixB[0].size(); j++)
				if (matrixB[k][j] == -1)
				{
					flag = false;

					for (int m = 0; m < notSelected_j.size(); m++)
						if (notSelected_j[m] == j)
						{
							flag = true;
							break;
						}

					if (!flag)
						notSelected_j.push_back(j);

					for (int i = 0; i < matrixB.size(); i++)
						if (matrixB[i][j] == 1)
						{
							flag = false;

							for (int m = 0; m < notSelected_i.size(); m++)
								if (notSelected_i[m] == i)
								{
									flag = true;
									break;
								}

							if (!flag)
								notSelected_i.push_back(i);
						}
				}
		}

	//--------------

	vector<int> selectedCopy(matrix.size(), -1);
	vector<vector<int>> M, B;
	vector<vector<int>> matrixCopy = matrix;
	egervariReduction(matrixCopy, notSelected_i, notSelected_j);

	reduction(matrixCopy);

	cout << "-- Egervari alg. " << num + 1 << " ---------------\n\n";
	cout << "   Matrix A'" << num + 1 << ":\n";
	matrixPrint(matrixCopy, 3);

	M = condidatesMatrixFind(matrixCopy);
	B = selectedCondidatesMatrixFind(M, selectedCopy);

	cout << "   Matrix M" << num + 1 << ":\n";
	matrixPrint(M, 3);
	cout << "   Matrix B" << num + 1 << ":\n";
	matrixPrint(B, 3);

	//--------------

	bool resultFlag = true, stopFlag;
	ChainVertex startVertex;

	for (int i = 0; i < matrix.size(); i++)
		if (selectedCopy[i] == -1)
		{
			resultFlag = false;

			startVertex.value = -1;
			startVertex.i = i;

			for (int j = 0; j < matrix[0].size(); j++)
				if (B[i][j] == -1)
				{
					startVertex.j = j;
					break;
				}
		}
	
	if (resultFlag)
	{
		cout << "== Result ========================\n";
		cout << "   Matrix B:\n";
		matrixPrint(B, 3);

	}
	else
	{
		stopFlag = chainCheck(B, startVertex, num);

		if (stopFlag)
			return;

		if (num < retryNum)
		{
			num++;
			egervariAlg(matrixCopy, B, selected, num, retryNum);
		}
		else
			cout << "== All vacancies alredy filled as much as they can.\n";
	}
}

//---------------------------------------------------------------

int main()
{
	vector<vector<int>> matrix, matrixCopy, M, B;
	matrix = matrixRead();
	matrixCopy = matrix;

	vector<int> selectedVacancies(matrix.size(), -1);

	cout << "Columns: candidates\nRows:\t vacancies\n\n";

	cout << "Matrix A:\n";
	matrixPrint(matrix, 0);

	inverseMatrix(matrixCopy);

	cout << "Matrix A':\n";
	matrixPrint(matrixCopy, 0);

	reduction(matrixCopy);

	cout << "Matrix A'(1&2):\n";
	matrixPrint(matrixCopy, 0);

	M = condidatesMatrixFind(matrixCopy);
	B = selectedCondidatesMatrixFind(M, selectedVacancies);

	cout << "Matrix M:\n";
	matrixPrint(M, 0);
	cout << "Matrix B:\n";
	matrixPrint(B, 0);

	//--------------

	int retryNum = 0;

	for (int i = 0; i < selectedVacancies.size(); i++)
		if (selectedVacancies[i] == -1)
			retryNum++;

	egervariAlg(matrixCopy, B, selectedVacancies, 0, retryNum);

	char ch;
	cin >> ch;
}
