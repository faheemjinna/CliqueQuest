#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to check compatibility between two input vectors
int isCompatible(const char *inputVector1, const char *inputVector2, int inputVectorLength)
{
    for (int i = 0; i < inputVectorLength; i++)
    {
        if (inputVector1[i] != inputVector2[i] && inputVector1[i] != 'X' && inputVector2[i] != 'X')
        {
            return 0; // Not compatible
        }
    }
    return 1; // Compatible
}

// Function to read input vectors from the input file
char **readInputVectorsFromInputFile(const char *inputFileName, int *inputVectorsCount, int inputVectorLength)
{
    FILE *file = fopen(inputFileName, "r");
    if (!file)
    {
        printf("Error: Could not open file %s\n", inputFileName);
        exit(1);
    }

    char **vectors = (char **)malloc(20000 * sizeof(char *));
    if (!vectors)
    {
        printf("Memory allocation failed for vectors.\n");
        exit(1);
    }
    *inputVectorsCount = 0;
    char line[inputVectorLength + 2];

    while (fscanf(file, "%s", line) != EOF)
    {
        vectors[*inputVectorsCount] = (char *)malloc((inputVectorLength + 1) * sizeof(char));
        if (!vectors[*inputVectorsCount])
        {
            printf("Memory allocation failed for vector %d.\n", *inputVectorsCount);
            exit(1);
        }
        strcpy(vectors[*inputVectorsCount], line);
        (*inputVectorsCount)++;
    }
    fclose(file);
    return vectors;
}

// Function to build a compatibility graph
int **constructGraph(char **vectors, int inputVectorsCount, int inputVectorLength)
{
    int **graph = (int **)malloc(inputVectorsCount * sizeof(int *));
    for (int i = 0; i < inputVectorsCount; i++)
    {
        graph[i] = (int *)calloc(inputVectorsCount, sizeof(int));
        if (!graph[i])
        {
            printf("Memory allocation failed for graph row %d.\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < inputVectorsCount; i++)
    {
        for (int j = i + 1; j < inputVectorsCount; j++)
        {
            if (isCompatible(vectors[i], vectors[j], inputVectorLength))
            {
                graph[i][j] = graph[j][i] = 1;
            }
        }
    }
    return graph;
}

// Function to find the largest clique in the current graph
int *largestClique(int **graph, int inputVectorsCount, int *available, int *cliqueSize)
{
    int *clique = (int *)malloc((inputVectorsCount + 1) * sizeof(int));
    *cliqueSize = 0;

    for (int v = 0; v < inputVectorsCount; v++)
    {
        if (!available[v])
            continue;

        int tempClique[inputVectorsCount];
        int tempSize = 0;
        tempClique[tempSize++] = v;

        for (int i = 0; i < inputVectorsCount; i++)
        {
            if (graph[v][i] == 1 && available[i])
            {
                int canAdd = 1;
                for (int j = 0; j < tempSize; j++)
                {
                    if (graph[tempClique[j]][i] == 0)
                    {
                        canAdd = 0;
                        break;
                    }
                }
                if (canAdd)
                    tempClique[tempSize++] = i;
            }
        }

        if (tempSize > *cliqueSize)
        {
            *cliqueSize = tempSize;
            for (int i = 0; i < tempSize; i++)
            {
                clique[i] = tempClique[i];
            }
            clique[tempSize] = -1;
        }
    }
    return clique;
}

// Function to remove a clique from the graph
void removeCliquesFromGraph(int *clique, int **graph, int inputVectorsCount, int *available)
{
    for (int i = 0; clique[i] != -1; i++)
    {
        int vertex = clique[i];
        available[vertex] = 0;
        for (int j = 0; j < inputVectorsCount; j++)
        {
            graph[vertex][j] = graph[j][vertex] = 0;
        }
    }
}

// Function to merge vectors in a clique to a compressed value
char *mergeClique(char **vectors, int *clique, int inputVectorLength)
{
    char *merged = (char *)malloc((inputVectorLength + 1) * sizeof(char));
    for (int i = 0; i < inputVectorLength; i++)
    {
        int hasOne = 0, hasZero = 0;
        for (int j = 0; clique[j] != -1; j++)
        {
            if (vectors[clique[j]][i] == '1')
            {
                hasOne = 1;
                break;
            }
            else if (vectors[clique[j]][i] == '0')
            {
                hasZero = 1;
            }
        }
        merged[i] = hasOne ? '1' : (hasZero ? '0' : 'X');
    }
    merged[inputVectorLength] = '\0';
    return merged;
}

void outputCliquesToFile(const char *outputFileName, int **cliques, int numCliques, char **vectors, int inputVectorLength)
{
    FILE *file = fopen(outputFileName, "w");
    if (!file)
    {
        printf("Error: Could not open or create file %s for writing\n", outputFileName);
        exit(1);
    }

    //inorder to check the values uncomment this and try running it

    // int count;
    for (int i = 0; i < numCliques; i++)
    {
        // count = 0;
        // fprintf(file, "Clique %d: {", i + 1);
        for (int j = 0; cliques[i][j] != -1; j++)
        {
            // count++;
            // fprintf(file, "%d", cliques[i][j] + 1);
            if (cliques[i][j + 1] != -1)
            {
                // fprintf(file, ", ");
            }
        }
        // fprintf(file, "}\n");
        // fprintf(file, "Clique %d size: %d\n", i + 1, count);

        char *compressedValue = mergeClique(vectors, cliques[i], inputVectorLength);
        fprintf(file, "Clique %d: %s\n", i + 1, compressedValue);
        free(compressedValue);
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage: ./dict <input_file> <max_cliques> <vector_length> <output_file>\n");
        return 1;
    }

    const char *inputFile = argv[1];
    int maxCliques = atoi(argv[2]);
    int vectorLength = atoi(argv[3]);
    const char *outputFile = argv[4];

    if (vectorLength != 8 && vectorLength != 16 && vectorLength != 32 && vectorLength != 64)
    {
        printf("Error: Invalid vector length. Must be 8, 16, 32, or 64.\n");
        return 1;
    }

    int inputVectorsCount;
    char **vectors = readInputVectorsFromInputFile(inputFile, &inputVectorsCount, vectorLength);
    // printf("%d %d\n",actualVectorCount, inputVectorsCount);
    int **graph = constructGraph(vectors, inputVectorsCount, vectorLength);
    int *available = (int *)calloc(inputVectorsCount, sizeof(int));
    for (int i = 0; i < inputVectorsCount; i++)
        available[i] = 1;

    int **cliques = (int **)malloc(maxCliques * sizeof(int *));
    int numCliques = 0;

    while (numCliques < maxCliques)
    {
        int cliqueSize;
        int *clique = largestClique(graph, inputVectorsCount, available, &cliqueSize);

        if (cliqueSize == 0)
        {
            free(clique);
            break;
        }

        cliques[numCliques++] = clique;
        removeCliquesFromGraph(clique, graph, inputVectorsCount, available);
    }

    if (numCliques < maxCliques)
    {
        printf("Only %d dictionary entries are possible\n", numCliques);
    }

    outputCliquesToFile(outputFile, cliques, numCliques, vectors, vectorLength);

    for (int i = 0; i < inputVectorsCount; i++)
    {
        free(vectors[i]);
        free(graph[i]);
    }
    free(vectors);
    free(graph);
    free(available);

    for (int i = 0; i < numCliques; i++)
    {
        free(cliques[i]);
    }
    free(cliques);

    return 0;
}