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

    char **vectors = (char **)malloc(20000 * sizeof(char *)); // Adjust size if necessary
    if (!vectors)
    {
        printf("Memory allocation failed for vectors.\n");
        exit(1);
    }
    *inputVectorsCount = 0;
    char line[inputVectorLength + 2]; // +2 for '\n' and '\0'

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

// Function to construct the graph based on compatibility
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

// Function to find the vertex with the maximum degree
int findMaxDegreeVertex(int **graph, int *available, int inputVectorsCount)
{
    int maxDegree = -1;
    int maxVertex = -1;
    for (int v = 0; v < inputVectorsCount; v++)
    {
        if (available[v])
        {
            int degree = 0;
            for (int i = 0; i < inputVectorsCount; i++)
            {
                if (graph[v][i] == 1)
                {
                    degree++;
                }
            }
            if (degree > maxDegree)
            {
                maxDegree = degree;
                maxVertex = v;
            }
        }
    }
    return maxVertex;
}

// Function to build a clique based on a vertex with the maximum degree
int *buildClique(int **graph, int *available, int maxVertex, int inputVectorsCount)
{
    int *clique = (int *)malloc(inputVectorsCount * sizeof(int));
    int cliqueSize = 0;

    // Add the vertex with max degree to the clique
    clique[cliqueSize++] = maxVertex;

    // Find all connected vertices and add to the clique
    for (int v = 0; v < inputVectorsCount; v++)
    {
        if (graph[maxVertex][v] == 1 && available[v])
        {
            clique[cliqueSize++] = v;
        }
    }

    // Terminate clique with -1 to indicate the end of the list
    clique[cliqueSize] = -1;
    return clique;
}

// Function to remove a clique from the graph
void removeCliqueFromGraph(int **graph, int *clique, int inputVectorsCount)
{
    for (int i = 0; clique[i] != -1; i++)
    {
        int vertex = clique[i];
        // Remove the vertex from all other vertices in the graph
        for (int j = 0; j < inputVectorsCount; j++)
        {
            graph[vertex][j] = graph[j][vertex] = 0;
        }
    }
}

// Function to find all cliques using the greedy heuristic
void findCliques(int **graph, int inputVectorsCount, int maxCliques, int **cliques, int *numCliquesFound)
{
    int *available = (int *)calloc(inputVectorsCount, sizeof(int));
    int numCliques = 0;

    // Mark all vertices as available
    for (int i = 0; i < inputVectorsCount; i++)
    {
        available[i] = 1;
    }

    while (numCliques < maxCliques)
    {
        // Find the vertex with the maximum degree
        int maxVertex = findMaxDegreeVertex(graph, available, inputVectorsCount);
        if (maxVertex == -1)
        {
            break; // No more vertices to process
        }

        // Build a clique based on the maximum degree vertex
        int *clique = buildClique(graph, available, maxVertex, inputVectorsCount);
        cliques[numCliques++] = clique;

        // Remove the clique from the graph
        removeCliqueFromGraph(graph, clique, inputVectorsCount);

        // Mark the vertices in the clique as unavailable
        for (int i = 0; clique[i] != -1; i++)
        {
            available[clique[i]] = 0;
        }
    }

    *numCliquesFound = numCliques;
    free(available);
}

// Function to write cliques to the output file
void outputCliquesToFile(const char *outputFileName, int **cliques, int num_cliques)
{
    FILE *file = fopen(outputFileName, "w");
    if (!file)
    {
        printf("Error: Could not open file %s for writing\n", outputFileName);
        exit(1);
    }

    for (int i = 0; i < num_cliques; i++)
    {
        fprintf(file, "Clique %d: {", i + 1);
        for (int j = 0; cliques[i][j] != -1; j++)
        {
            fprintf(file, "%d", cliques[i][j] + 1);
            if (cliques[i][j + 1] != -1)
            {
                fprintf(file, ", ");
            }
        }
        fprintf(file, "}\n");
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage: ./program <input_file> <max_cliques> <vector_length> <output_file>\n");
        return 1;
    }

    const char *inputFile = argv[1];
    int maxCliques = atoi(argv[2]);
    int vectorLength = atoi(argv[3]);
    const char *outputFile = argv[4];

    int inputVectorsCount;
    char **vectors = readInputVectorsFromInputFile(inputFile, &inputVectorsCount, vectorLength);

    int **graph = constructGraph(vectors, inputVectorsCount, vectorLength);

    int **cliques = (int **)malloc(maxCliques * sizeof(int *));
    int numCliquesFound;

    findCliques(graph, inputVectorsCount, maxCliques, cliques, &numCliquesFound);

    outputCliquesToFile(outputFile, cliques, numCliquesFound);

    // Clean up
    for (int i = 0; i < inputVectorsCount; i++)
    {
        free(vectors[i]);
        free(graph[i]);
    }
    free(vectors);
    free(graph);

    for (int i = 0; i < numCliquesFound; i++)
    {
        free(cliques[i]);
    }
    free(cliques);

    return 0;
}
