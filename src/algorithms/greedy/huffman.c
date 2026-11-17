
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

typedef struct HuffmanNode {
    unsigned char character;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

static HuffmanNode *create_node(unsigned char ch, int freq)
{
    HuffmanNode *node = malloc(sizeof(HuffmanNode));

    if (node == NULL) {
        return NULL;
    }

    node->character = ch;
    node->frequency = freq;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static void free_tree(HuffmanNode *root)
{
    if (root == NULL) return;

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static void print_codes(HuffmanNode *root, int code[], int depth)
{
    if (root == NULL) return;

    if (root->left == NULL && root->right == NULL) {
        printf("Character: ");

        if (root->character == ' ') {
            printf("[space]");
        } else {
            printf("%c", root->character);
        }

        printf(" | Code: ");

        if (depth == 0) {
            printf("0");
        } else {
            for (int i = 0; i < depth; i++) {
                printf("%d", code[i]);
            }
        }

        printf("\n");
        return;
    }

    code[depth] = 0;
    print_codes(root->left, code, depth + 1);

    code[depth] = 1;
    print_codes(root->right, code, depth + 1);
}

void huffman_compress(const char *text)
{
    if (text == NULL || text[0] == '\0') {
        printf("No text available for compression.\n");
        return;
    }

    int frequency[256] = {0};

    for (size_t i = 0; text[i] != '\0'; i++) {
        frequency[(unsigned char)text[i]]++;
    }

    HuffmanNode *nodes[256];
    int count = 0;

    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            nodes[count] = create_node((unsigned char)i, frequency[i]);

            if (nodes[count] == NULL) {
                for (int j = 0; j < count; j++) {
                    free_tree(nodes[j]);
                }
                printf("Memory allocation failed.\n");
                return;
            }

            count++;
        }
    }

    while (count > 1) {
        int first = 0;
        int second = 1;

        if (nodes[second]->frequency < nodes[first]->frequency) {
            int temp = first;
            first = second;
            second = temp;
        }

        for (int i = 2; i < count; i++) {
            if (nodes[i]->frequency < nodes[first]->frequency) {
                second = first;
                first = i;
            } else if (nodes[i]->frequency < nodes[second]->frequency) {
                second = i;
            }
        }

        HuffmanNode *parent = create_node(
            0,
            nodes[first]->frequency + nodes[second]->frequency
        );

        if (parent == NULL) {
            for (int i = 0; i < count; i++) {
                free_tree(nodes[i]);
            }
            printf("Memory allocation failed.\n");
            return;
        }

        parent->left = nodes[first];
        parent->right = nodes[second];

        if (first > second) {
            int temp = first;
            first = second;
            second = temp;
        }

        nodes[first] = parent;
        nodes[second] = nodes[count - 1];
        count--;
    }

    int code[256];

    printf("\n============================================\n");
    printf("           HUFFMAN CODING MODULE\n");
    printf("============================================\n");

    printf("Original Text: %s\n\n", text);
    print_codes(nodes[0], code, 0);

    printf("============================================\n");

    free_tree(nodes[0]);
}
