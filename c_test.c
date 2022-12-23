#include "c_test.h"

static void u32_swap(void *a, void *b, size_t size)
{
    u32 t = *(u32 *)a;
    *(u32 *)a = *(u32 *)b;
    *(u32 *)b = t;
}

static void u64_swap(void *a, void *b, size_t size)
{
    u64 t = *(u64 *)a;
    *(u64 *)a = *(u64 *)b;
    *(u64 *)b = t;
}

static void generic_swap(void *a, void *b, size_t size)
{
    char t;
    while (size--) {
        t = *(char *)a;
        *(char *)a = *(char *)b;
        *(char *)b = t;
    }
}

void swap(char *a, char *b)
{
    char t;

    t = *a;  // x = a ^ b
    *a = *b;  // a' = x ^ b
    *b = t;  // b' = x ^ a'
}

static void printd(int n)
{
    if (n < 0) {
        printf("-");
        n = -n;
    }

    if (n >= 10)
        printd(n/10);

    printf("%c", n % 10 + '0');
}

void invert_string(char *s, size_t size)
{
    size_t i, j;

    if (!size)
        return;

    for (i = 0, j = size - 1; i < j; i++, j--)
        swap(s + i, s + j);
}

void invert_sentence(char *s)
{
    size_t i = 0;

    for (i = 0; s[i] != '\0'; i++)
        ;
    invert_string(s, i);

    i = 0;
    while (1) {
        if (s[i] == ' ' || s[i] == '\0') {
            invert_string(s, i);
            s = s + i + 1;
            i = 0;
            if (s[i] == '\0')
                break;
            else
                continue;
        }
        i++;
    }
}

struct PASSWORD_INFO
{
    int lowercase;
    int uppercase;
    int digit;
    int add;
    int del;
    int comma;
    int rmin;
    int rmax;
    int plen;
    int nlen;
};

#define ISUPPER(c)  (((c) >= 'A') && ((c) <= 'Z'))
#define ISLOWER(c)  (((c) >= 'a') && ((c) <= 'z'))
#define ISDIGIT(c)  (((c) >= '0') && ((c) <= '9'))

#define CONFIG_DEBUG_MESSAGE    (1)
#if (CONFIG_DEBUG_MESSAGE)
#define PRINT_PWDINFO(title, password, marker, repeat1, repeat2, pwdinfo) {\
    printf("\n%s\np: %s\nm: %s\nr: %s\n2: %s\n", title, password, marker, repeat1, repeat2); \
    printf("s: %d, n: %d, a: %d, d: %d, c: %d, l: %d, u: %d, d: %d r: %d %d\n", \
            pwdinfo.plen, pwdinfo.nlen, \
            pwdinfo.add, pwdinfo.del, \
            pwdinfo.comma, pwdinfo.lowercase, \
            pwdinfo.uppercase, pwdinfo.digit, \
            pwdinfo.rmin, pwdinfo.rmax);}
#else
#define PRINT_PWDINFO
#endif

void UpdatePasswordInfo(
    struct PASSWORD_INFO *pwdinfo,
    const char *password,
    char *marker,
    char *repeat1,
    char *repeat2)
{
    if (pwdinfo == NULL || password == NULL || marker == NULL || repeat1 == NULL || repeat2 == NULL)
        return;

    const char *p = password;
    char *m = marker, *r1 = repeat1, *r2 = repeat2, last = '\0';
    char i, j, k, rcnt0 = 1, rcnt1, rcnt2;

    memset(pwdinfo, 0, sizeof(struct PASSWORD_INFO));
    pwdinfo->rmin = INT_MAX;
    pwdinfo->rmax = INT_MIN;

    for (i = 0; p[i] != '\0'; i++) {

        pwdinfo->plen++;
        r1[i] = '_';
        r2[i] = '_';
        // If m[i] is a "DEL" marker, skip it.
        if (m[i] == '^')
            continue;

        // Calculate character repeat1 count
        for (j = i, k = i, rcnt1 = 0, rcnt2 = 0; p[j] != '\0'; j++)
            if (p[i] == p[j]) {
                rcnt1++;
                if (k != -1)
                    rcnt2++;
            } else
                k = -1;

        r1[i] = rcnt1 + '0';
        r2[i] = rcnt2 + '0';
        m[i] = '_';

        if (rcnt1 < pwdinfo->rmin)
            pwdinfo->rmin = rcnt1;

        if (rcnt1 > pwdinfo->rmax)
            pwdinfo->rmax = rcnt1;

        if (last != p[i]) {
            rcnt0 = 1;
            last = p[i];
        } else
            rcnt0++;

        if (rcnt0 && ((rcnt0 % 3) == 0)) {
            m[i] = ',';
            pwdinfo->comma++;
        }

        if (ISUPPER(p[i]))
            pwdinfo->uppercase++;

        if (ISLOWER(p[i]))
            pwdinfo->lowercase++;

        if (ISDIGIT(p[i]))
            pwdinfo->digit++;

        pwdinfo->nlen++;
    }

    m[i] = r1[i] = '\0';
    pwdinfo->del = pwdinfo->nlen > 20 ? pwdinfo->nlen - 20 : 0;
    pwdinfo->add = pwdinfo->nlen < 6 ? 6 - pwdinfo->nlen : 0;
    PRINT_PWDINFO("UpdatePasswordInfo", password, marker, repeat1, repeat2, (*pwdinfo));

    return;
}

int strongPasswordChecker(const char *password){

    // password = "a";
    // password = "aA1";
    // password = "1337C0d3";
    // password = "aaaB1";
    // password = "1111111111";
    // password = "ABABABABABABABABABAB1";   // 21
    // password = "bbaaaaaaaaaaaaaaacccccc";
    // password = "aaaaAAAAAA000000123456";  // 22
    password = "FFFFFFFFFFFFFFF11111111111111111111AAA";
    // password = "...";
    // password = "QQQQQ";

    struct PASSWORD_INFO pwdinfo;
    static char marker[50];
    static char repeat1[50];
    static char repeat2[50];
    char i, j, cnt, cnt2;

    memset(marker, '_', 50);
    memset(repeat1, '_', 50);

    UpdatePasswordInfo(&pwdinfo, password, marker, repeat1, repeat2);

    // Delete character
    while (pwdinfo.del > 0) {
        cnt2 = pwdinfo.comma > 0 ? CHAR_MAX : CHAR_MIN;
        for (i = 0, j = -1; i < pwdinfo.plen; i++) {
            if (pwdinfo.comma > 0) {
                if (marker[i] == ',') {
                    cnt = repeat2[i] - '0';
                    if (cnt < cnt2) {
                        cnt2 = cnt;
                        j = i;
                    }
                }
            } else {
                if (repeat1[i] == '_')
                    continue;

                if (ISUPPER(password[i]) && pwdinfo.uppercase == 1)
                    continue;

                if (ISLOWER(password[i]) && pwdinfo.lowercase == 1)
                    continue;

                if (ISDIGIT(password[i]) && pwdinfo.digit == 1)
                    continue;

                cnt = repeat1[i] - '0';
                if (pwdinfo.rmax <= 3 && cnt == 3)
                    continue;

                if ((cnt % 3) && (cnt > cnt2)) {
                    cnt2 = cnt;
                    j = i;
                }
            }
        }

        marker[j] = '^';
        PRINT_PWDINFO("DEL >>>", password, marker, repeat1, repeat2, pwdinfo);
        UpdatePasswordInfo(&pwdinfo, password, marker, repeat1, repeat2);
    }

    // Add character, case 1
    if (pwdinfo.nlen < 6) {
        if (pwdinfo.comma > 0) {
            for (i = 0; i < pwdinfo.plen; i++)
                if (marker[i] == ',')
                    marker[i] = '_';

            pwdinfo.add--;
            marker[pwdinfo.plen] = '$';
            pwdinfo.plen++;
            pwdinfo.nlen++;
            marker[pwdinfo.plen] = '\0';
            PRINT_PWDINFO("ADD1 >>>", password, marker, repeat1, repeat2, pwdinfo);
        }
    }

    // Add characters, case 2
    if (pwdinfo.nlen < 6) {
        memset(marker + pwdinfo.plen, '@', pwdinfo.add);
        pwdinfo.plen += pwdinfo.add;
        pwdinfo.nlen += pwdinfo.add;
        marker[pwdinfo.plen] = '\0';
        PRINT_PWDINFO("ADD2 >>>", password, marker, repeat1, repeat2, pwdinfo);
    }

    // Replace ',', '$', '@' to ? character
    cnt = ((pwdinfo.lowercase == 0) + (pwdinfo.uppercase == 0) + (pwdinfo.digit == 0));
    if ((pwdinfo.add + pwdinfo.comma) < cnt) {
        if ((pwdinfo.rmax < 3) && (20 - pwdinfo.nlen) >= cnt) {
            memset(marker + pwdinfo.plen, '?', cnt);
            pwdinfo.plen += cnt;
            pwdinfo.nlen += cnt;
            marker[pwdinfo.plen] = '\0';
            PRINT_PWDINFO("CHG1 >>>", password, marker, repeat1, repeat2, pwdinfo);
        } else {
            while (cnt) {
                for (i = 0; i < pwdinfo.plen; i++) {
                    if ((pwdinfo.add + pwdinfo.comma) > 0) {
                        if (marker[i] == ',' || marker[i] == '$') {
                            marker[i] = '?';
                            pwdinfo.comma--;
                            cnt--;
                            break;
                        }
                        if (marker[i] == '@') {
                            marker[i] = '?';
                            pwdinfo.add--;
                            cnt--;
                            break;
                        }
                    } else {
                        if (marker[i] == '_') {
                            marker[i] = '?';
                            cnt--;
                            break;
                        }
                    }
                }
            }

            PRINT_PWDINFO("CHG2 >>>", password, marker, repeat1, repeat2, pwdinfo);
        }
    }

    for (i = 0, cnt = 0; i < pwdinfo.plen && marker[i] != '\0'; i++)
        if (marker[i] != '_')
            cnt++;

    return cnt;
}

int _strcmp(const char *cs, const char *ct)
{
    unsigned char c1, c2;

    while (1) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;

        if (!c1)
            break;
    }

    return 0;
}

char * simplifyPath(char * path){

    if (path == NULL)
        return NULL;

    char *p = path, *np, *t;
    int i, j, plen;

    while (*p != '\0')
        p++;

    plen = p - path;
    np = malloc(plen + 1);
    memset(np, 0, plen);
    p = np;

    for (i = 0; i < plen; i++) {
        // If meet a '/' char
        if (path[i] == '/') {
            // Skip following '/' chars
            for (j = i + 1; j < plen; j++)
                if (path[j] != '/')
                    break;

            // If the next char is '\0', stop the process.
            if (path[j] == '\0')
                break;

            t = p;
            *t++ = path[i];

            for (; j < plen; j++) {
                if (path[j] == '/')
                    break;
                else
                    *t++ = path[j];
            }
            *t = '\0';

            if (_strcmp(p, "/..") == 0) {
                while (p != np)
                    if (*--p == '/')
                        break;
            } else if (_strcmp(p, "/.") != 0)
                p = t;

            i = j - 1;
        }
    }

    if (p == np)
        *p++ = '/';
    *p = '\0';

    return np;
}

#if 0
void RemoveExcessiveOpens(char **ans, int *ansLen) {
    int shift = 0;
    int numClosed = 0;
    char *workingStr = *ans;

    for (int i = *ansLen - 1; i >= 0; i--) {
        workingStr[i + shift] = workingStr[i];

        if(workingStr[i] == ')') {
            numClosed++;
        } else if (workingStr[i] == '(') {
            if (numClosed > 0) {
                numClosed--;
            } else {
                shift++;
                (*ansLen)--;
            }
        }
    }
    // fix any gap in front of the string
    *ans += shift;
}

void RemoveExcessiveCloses(char *s, int *ansLen) {
    int shift = 0;
    int numOpen = 0;
    const int stringLen = *ansLen;

    for(int i = 0; i < stringLen; i++) {
        s[i - shift] = s[i];

        if(s[i] == '(') {
            numOpen++;
        } else if (s[i] == ')') {
            if (numOpen > 0) {
                numOpen--;
            } else {
                shift++;
                (*ansLen)--;
            }
        }
    }
}

char * minRemoveToMakeValid(char * s){
    int ansLen = strlen(s);
    char *ans = s;

    RemoveExcessiveOpens(&ans, &ansLen);
    RemoveExcessiveCloses(ans, &ansLen);

    ans[ansLen] = 0;
    return ans;
}
#else

// char * minRemoveToMakeValid(char * s)
// {
//     int i, j, slen, rp, lp, shift;

//     char *p = s;
//     while (*p++ != '\0');
//     slen = p - s - 1;
//     // slen = strlen(s);

//     // remove redundant lp
//     rp = lp = shift = 0;
//     for (i = slen - 1; i >= 0; i--) {
//         s[i + shift] = s[i];

//         if (s[i] == ')')
//             rp++;
//         else if (s[i] == '(') {
//             if (rp)
//                 rp--;
//             else {
//                 shift++;
//                 slen--;
//             }
//         }
//     }
//     s = s + shift;

//     rp = lp = shift = 0;
//     j = slen;
//     for (i = 0; i < j; i++) {
//         s[i - shift] = s[i];

//         if (s[i] == '(')
//             lp++;
//         else if (s[i] == ')') {
//             if (lp)
//                 lp--;
//             else {
//                 shift++;
//                 slen--;
//             }
//         }
//     }
//     s[slen] = '\0';

//     return s;
// }

#if 1
char * minRemoveToMakeValid(char * s)
{
    #define LESS_MEMORY (1)
    int i, slen, qf, qr;
    int parentheses_cnt, invalid_parentheses_cnt;
    int *ipqueue;
    char *p;

    // Calculate slen and parentheses count
    p = s;
    i = parentheses_cnt = invalid_parentheses_cnt = 0;
    while (*p != '\0') {
        if (*p == '(') {
            parentheses_cnt++;
            i++;
        }

        if (*p == ')') {
            parentheses_cnt++;
            if (i == 0)
                invalid_parentheses_cnt++;
            else
                i--;
        }
        p++;
    }

    invalid_parentheses_cnt += i;
    slen = p - s;
    if (slen == invalid_parentheses_cnt)
        return "";

    ipqueue = malloc(sizeof(int) * (invalid_parentheses_cnt + ((parentheses_cnt - invalid_parentheses_cnt) >> 1)));
#if (LESS_MEMORY == 0)
    char *ns = malloc(slen + 1);
#endif

    // Get positions of invalid parentheses
    p = s;
    qr = i = 0;
    while (*p != '\0') {
        if (*p == '(') {
            ipqueue[qr++] = p - s;
            i++;
        }

        if (*p == ')') {
            if (i == 0)
                ipqueue[qr++] = p - s;
            else {
                qr--;
                i--;
            }
        }
        p++;
    }

    // Copy valid string
#if (LESS_MEMORY == 0)
    int j;
    i = j = qf = 0;
    while (qf != qr) {
        memcpy(ns + i, s + j, ipqueue[qf] - j);
        i = i + (ipqueue[qf] - j);
        j = ipqueue[qf] + 1;
        qf++;
    }

    if (j < slen) {
        memcpy(ns + i, s + j, slen - j);
        i = i + (slen - j);
    }
    ns[i] = '\0';
    return ns;
#else
    // Copy valid string
    for (i = 0, qf = 0, p = s; s[i] != '\0'; i++) {
        if ((qf != qr) && (i == ipqueue[qf])) {
            qf++;
            continue;
        }
        *p++ = s[i];
    }
    *p = '\0';
    return s;
#endif
}
#endif
#endif

static int int_comp(const void *a, const void *b, size_t size)
{
    // printf("int_comp %2d %5d %5d\n", *(int *)a - *(int *)b, *(int *)a, *(int *)b);
    return *(int *)a - *(int *)b;
}

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize)
{
    int i, k, *p;
    int *r = malloc(sizeof(int) * 2);
    int *inums = malloc(sizeof(int) * numsSize);
    for (i = 0; i < numsSize; i++)
        inums[i] = i;
    heap_sort(nums, inums, numsSize, sizeof(*nums), int_comp, NULL);

    for (i = 0; i < numsSize - 1; i++) {
        k = target - nums[i];
        printf("%d %d\n", i, k);
        p = binary_search(nums + (i + 1), &k, sizeof(*nums), numsSize - (i + 1), int_comp);
        if (p != NULL)
        {
            r[0] = inums[i];
            r[1] = inums[p - nums];
            *returnSize = 2;
            printf("r[0]=%d, r[1]=%d %d %d\n", r[0], r[1], (p - nums), sizeof(*nums));
            free(inums);
            return r;
        }
    }

    return r;
}

/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int** threeSum(int* nums, int numsSize, int* returnSize, int** returnColumnSizes){

    *returnSize = 0;
    if (nums == NULL || numsSize < 3)
        return NULL;

    int memsize = 2;
    int **results = malloc(sizeof(int *) * memsize);
    (*returnColumnSizes) = malloc(sizeof(int) * memsize);

    heap_sort(nums, 0, numsSize, sizeof(*nums), int_comp, u32_swap);

    int i, left, right, sum, temp;

    for (i = 0; i < numsSize - 2; i++) {
        if (nums[i] > 0 || (nums[i] + nums[i + 1] + nums[i + 2]) > 0)
            break;

        if (i > 0 && (nums[i - 1] == nums[i]))
            continue;

        left = i + 1;
        right = numsSize - 1;
        sum = -nums[i];

        while (left < right) {
            temp = nums[left] + nums[right];

            if (temp < sum)
                left++;
            else if (temp > sum)
                right--;
            else {
                results[(*returnSize)] = malloc(sizeof(int) * 3);
                results[(*returnSize)][0] = nums[i];
                results[(*returnSize)][1] = nums[left++];
                results[(*returnSize)][2] = nums[right--];
                (*returnColumnSizes)[(*returnSize)] = 3;
                (*returnSize)++;

                while (left < right && (nums[left - 1] == nums[left]))
                    left++;

                while (right > left && (nums[right + 1] == nums[right]))
                    right--;

                if ((*returnSize) == memsize) {
                    memsize *= 2;
                    (*returnColumnSizes) = realloc((*returnColumnSizes), sizeof(int) * memsize);
                    results = realloc(results, sizeof(int *) * memsize);
                }
            }
        }
    }

    return results;
}

/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int** fourSum(int* nums, int numsSize, int target, int* returnSize, int** returnColumnSizes){
    *returnSize = 0;
    if (nums == NULL || numsSize < 4)
        return NULL;

    printf("numsSize: %d\n", numsSize);
    int memsize = 2;
    int **results = malloc(sizeof(int *) * memsize);
    (*returnColumnSizes) = malloc(sizeof(int) * memsize);

    heap_sort(nums, 0, numsSize, sizeof(*nums), int_comp, u32_swap);

    int i, j, left, right, sum, temp;

    for (i = 0; i < (numsSize > 100 ? 100 : numsSize); i++) {
        if (i && (i % 8) == 0)
            printf("\n");

        printf("%2d ", nums[i]);
    }
    printf("\n");

    for (i = 0; i < numsSize - 3; i++) {
        if (((long long)nums[i] + (long long)nums[i + 1] +
             (long long)nums[i + 2] + (long long)nums[i + 3]) > target)
            break;

        if ((target > 0) && (nums[i] > target))
            break;

        if ((target < 0) && (nums[i] > 0))
            break;

        if (i > 0 && (nums[i - 1] == nums[i]))
            continue;

        sum = target - nums[i];

        for (j = i + 1; j < numsSize - 2; j++) {

            if (j > (i + 1) && (nums[j - 1] == nums[j]))
                continue;

            sum = target - nums[i] - nums[j];

            left = j + 1;
            right = numsSize - 1;

            while (left < right) {
                printf("i: %2d %2d %2d %2d\n", i, j, left, right);
                printf("v: %2d %2d %2d %2d\n", nums[i], nums[j], nums[left], nums[right]);
                temp = nums[left] + nums[right];

                if (temp < sum)
                    left++;
                else if (temp > sum)
                    right--;
                else {
                    printf("^^^^^^^^^^^^^^\n\n");
                    results[(*returnSize)] = malloc(sizeof(int) * 4);
                    results[(*returnSize)][0] = nums[i];
                    results[(*returnSize)][1] = nums[j];
                    results[(*returnSize)][2] = nums[left++];
                    results[(*returnSize)][3] = nums[right--];
                    (*returnColumnSizes)[(*returnSize)] = 4;
                    (*returnSize)++;

                    while (left < right && (nums[left - 1] == nums[left]))
                        left++;

                    while (right > left && (nums[right + 1] == nums[right]))
                        right--;

                    if ((*returnSize) == memsize) {
                        memsize *= 2;
                        (*returnColumnSizes) = realloc((*returnColumnSizes), sizeof(int) * memsize);
                        results = realloc(results, sizeof(int *) * memsize);
                    }
                }
            }
        }
    }

    return results;
}

// double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size){

//     int i, n1, n2, n3, nums3Size;
//     int *nums3;

//     if (nums1 == NULL && nums2 == NULL)
//         return 0;

//     nums3Size = (nums1Size + nums2Size);
//     n3 = (nums3Size >> 1) + 1;
//     nums3 = malloc(sizeof(int) * n3);

//     for (i = 0, n1 = 0, n2 = 0; i < n3; i++) {
//         if (n1 < nums1Size && n2 < nums2Size) {
//             if (nums1[n1] < nums2[n2]) {
//                 nums3[i] = nums1[n1++];
//             } else {
//                 nums3[i] = nums2[n2++];
//             }
//         } else if (n1 < nums1Size) {
//             nums3[i] = nums1[n1++];
//         } else if (n2 < nums2Size) {
//             nums3[i] = nums2[n2++];
//         }
//     }

//     return nums3Size % 2 ?
//             nums3[n3 - 1] :
//             ((double)nums3[n3 - 1] + (double)nums3[n3 - 2]) / 2;
// }


double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size){

    if (nums1 == NULL && nums2 == NULL)
        return 0;

    int i, n1 = 0, n2 = 0;
    double m1 = 0, m2 = 0;

    for (i = 0; i <= ((nums1Size + nums2Size) >> 1); i++) {
        m2 = m1;
        if (n1 < nums1Size && n2 < nums2Size) {
            if (nums1[n1] < nums2[n2])
                m1 = nums1[n1++];
            else
                m1 = nums2[n2++];
        } else if (n1 < nums1Size)
            m1 = nums1[n1++];
        else
            m1 = nums2[n2++];
    }

    // odd count
    if ((nums1Size + nums2Size) % 2)
        return m1;

    // even count
    return (m1 + m2) / 2;
}

// double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size){

//     short i, n1, n2;//, n3, nums3Size;
//     // int *nums3;

//     if (nums1 == NULL && nums2 == NULL)
//         return 0;

//     // nums3Size = (nums1Size + nums2Size);
//     // n3 = (nums3Size >> 1) + 1;
//     // nums3 = malloc(sizeof(int) * n3);
//     char i1 = 0, i2 = 0;

//     for (i = 0, n1 = 0, n2 = 0; i < ((nums1Size + nums2Size) >> 1) + 1; i++) {
//         i2 = i1;
//         if (n1 < nums1Size && n2 < nums2Size) {
//             if (nums1[n1] < nums2[n2]) {
//                 // nums3[i] = nums1[n1++];
//                 n1++;
//                 i1 = 1;
//             } else {
//                 // nums3[i] = nums2[n2++];
//                 n2++;
//                 i1 = 2;
//             }
//         } else if (n1 < nums1Size) {
//             // nums3[i] = nums1[n1++];
//             n1++;
//             i1 = 1;
//         } else if (n2 < nums2Size) {
//             // nums3[i] = nums2[n2++];
//             n2++;
//             i1 = 2;
//         }
//     }

//     // return nums3Size % 2 ?
//     //         nums3[n3 - 1] :
//     //         ((double)nums3[n3 - 1] + (double)nums3[n3 - 2]) / 2;

//     // odd count
//     if ((nums1Size + nums2Size) % 2) {
//         if (i1 == 1) {
//             return nums1[n1 - 1];
//         } else {
//             return nums2[n2 - 1];
//         }
//     }
//     // even count
//     else {
//         if (i1 == 1) {
//             if (i2 == 1)
//                 return ((double)nums1[n1 - 1] + (double)nums1[n1 - 2]) / 2;
//             else
//                 return ((double)nums1[n1 - 1] + (double)nums2[n2 - 1]) / 2;
//         } else {
//             if (i2 == 1)
//                 return ((double)nums2[n2 - 1] + (double)nums1[n1 - 1]) / 2;
//             else
//                 return ((double)nums2[n2 - 1] + (double)nums2[n2 - 2]) / 2;
//         }
//     }
// }

// double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size){

//     int i, n1, n2, n3, nums3Size;
//     // int *nums3;

//     if (nums1 == NULL && nums2 == NULL)
//         return 0;

//     nums3Size = (nums1Size + nums2Size);
//     n3 = (nums3Size >> 1) + 1;
//     // nums3 = malloc(sizeof(int) * n3);
//     int i1 = 0, i2 = 0;

//     for (i = 0, n1 = 0, n2 = 0; i < n3; i++) {
//         i2 = i1;
//         if (n1 < nums1Size && n2 < nums2Size) {
//             if (nums1[n1] < nums2[n2]) {
//                 // nums3[i] = nums1[n1++];
//                 n1++;
//                 i1 = 1;
//             } else {
//                 // nums3[i] = nums2[n2++];
//                 n2++;
//                 i1 = 2;
//             }
//         } else if (n1 < nums1Size) {
//             // nums3[i] = nums1[n1++];
//             n1++;
//             i1 = 1;
//         } else if (n2 < nums2Size) {
//             // nums3[i] = nums2[n2++];
//             n2++;
//             i1 = 2;
//         }
//     }

//     // return nums3Size % 2 ?
//     //         nums3[n3 - 1] :
//     //         ((double)nums3[n3 - 1] + (double)nums3[n3 - 2]) / 2;

//     // odd count
//     if (nums3Size % 2) {
//         if (i1 == 1) {
//             return nums1[n1 - 1];
//         } else {
//             return nums2[n2 - 1];
//         }
//     }
//     // even count
//     else {
//         if (i1 == 1) {
//             if (i2 == 1)
//                 return ((double)nums1[n1 - 1] + (double)nums1[n1 - 2]) / 2;
//             else
//                 return ((double)nums1[n1 - 1] + (double)nums2[n2 - 1]) / 2;
//         } else {
//             if (i2 == 1)
//                 return ((double)nums2[n2 - 1] + (double)nums1[n1 - 1]) / 2;
//             else
//                 return ((double)nums2[n2 - 1] + (double)nums2[n2 - 2]) / 2;
//         }
//     }
// }

// double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size){

//     int i, n1, n2, nums3Size = (nums1Size + nums2Size);
//     int *nums3 = malloc(sizeof(int) * (nums1Size + nums2Size));

//     if (nums1 == NULL && nums2 == NULL)
//         return 0;

//     for (i = 0, n1 = 0, n2 = 0; i < nums3Size; i++) {

//         if (n1 < nums1Size && n2 < nums2Size) {
//             if (nums1[n1] < nums2[n2]) {
//                 nums3[i] = nums1[n1++];
//             } else {
//                 nums3[i] = nums2[n2++];
//             }
//         } else if (n1 < nums1Size) {
//             nums3[i] = nums1[n1++];
//         } else if (n2 < nums2Size) {
//             nums3[i] = nums2[n2++];
//         }
//     }

//     return nums3Size % 2 ?
//             nums3[nums3Size >> 1] :
//             ((double)nums3[nums3Size >> 1] + (double)nums3[(nums3Size >> 1) - 1]) / 2;
// }


 struct ListNode {
     int val;
     struct ListNode *next;
 };

// struct ListNode* mergeKLists(struct ListNode** lists, int listsSize){

//     struct ListNode **min_node;
//     struct ListNode *head = NULL, *tail = NULL;
//     int min_val;

//     while (1) {
//         min_node = NULL;
//         min_val = INT_MAX;
//         for (int i = 0; i < listsSize; i++) {
//             if (lists[i] != NULL) {
//                 if (lists[i]->val < min_val) {
//                     min_val = lists[i]->val;
//                     min_node = &lists[i];
//                 }
//             }
//         }

//         if (min_node != NULL) {
//             if (head == NULL) {
//                 head = (*min_node);
//                 tail = head;
//             } else {
//                 tail->next = (*min_node);
//                 tail = (*min_node);
//             }
//             (*min_node) = (*min_node)->next;
//         } else
//             break;
//     }

//     return head;
// }

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */

static struct ListNode *merge2Lists(struct ListNode *list1, struct ListNode *list2)
{
    struct ListNode *iter1 = list1, *iter2 = list2;
    struct ListNode *merge_head, *node;

    if (list1 == NULL)
        return list2;

    if (list2 == NULL)
        return list1;

    if (list1 == list2)
        return list1;

    if (iter1->val < iter2->val) {
        merge_head = iter1;
        iter1 = iter1->next;
    } else {
        merge_head = iter2;
        iter2 = iter2->next;
    }

    node = merge_head;

    while (iter1 && iter2) {
        if (iter1->val < iter2->val) {
            node->next = iter1;
            iter1 = iter1->next;
        } else {
            node->next = iter2;
            iter2 = iter2->next;
        }
        node = node->next;
    }

    while (iter1) {
        node->next = iter1;
        iter1 = iter1->next;
        node = node->next;
    }

    while (iter2) {
        node->next = iter2;
        iter2 = iter2->next;
        node = node->next;
    }

    return merge_head;
}

struct ListNode* mergeKLists(struct ListNode** lists, int listsSize){

    if (listsSize == 0)
        return NULL;

    if (listsSize == 1)
        return lists[0];

    int i, size;

    while (listsSize > 1) {
        size = listsSize;
        for (i = 0; i < (size >> 1); i++) {
            lists[i] = merge2Lists(lists[i], lists[size - i - 1]);
            listsSize--;
        }
    }

    return lists[0];
}

#if 1
int lengthOfLongestSubstring(char * s){

    if (s == NULL)
        return 0;

    if (*s == '\0')
        return 0;

    int max_cnt = INT_MIN;
    char *cs = s, *cc;
    char last = '\0';

    while (*s != '\0') {
        if (cs < s) {
            cc = cs;
            while (cc < s) {
                if (*cc == *s) {
                    if ((s - cs) > max_cnt)
                        max_cnt = (s - cs);
                    cs = cc + 1;
                    break;
                }
                cc++;
            }
        }

        if (*s != last)
            last = *s;
        else {
            if ((s - cs) > max_cnt)
                max_cnt = (s - cs);
            cs = s;
        }
        s++;
    }

    if ((s - cs) > max_cnt)
        max_cnt = (s - cs);

    return max_cnt;
}
#else
int lengthOfLongestSubstring(char * s){

    // s = "abcabcbb";
    // s = "bbbbb";
    // s = "dvdf";
    // s = "abcdeabcde";   // 5
    // s = "abcdeabcd";    // 5
    // s = "abcdebcde";    // 5
    // s = "abcdebcdefg";  // 6
    // s = "abcdeefg";     // 5

    // s = "abcdedcba";   // 5
    // s = "abcdedfghijk";   // 8
    // s = "aab";
    // s = "abb";
    // s = "aaabb";
    // s = "abbb";

    if (s == NULL)
        return 0;

    if (*s == '\0')
        return 0;

    char *ls = s, *le = s, *cs = s, *ce = s, *cc;
    char last = '\0';

    while (*s != '\0') {

        printf("s: %c\n", *s);

        if (ce != NULL) {
            cc = cs;
            while (cc < ce) {
                if (*cc == *ce) {
                    printf("1> %d %d\n\n", (ce - cs), (le - ls));
                    if ((ce - cs) > (le - ls)) {
                        le = ce;
                        ls = cs;
                    }
                    cs = cc + 1;
                    break;
                }
                cc++;
            }
        }

        if (*s != last) {
            printf("l %c\n", last);
            last = *s;
        } else {
            printf("2> %d %d\n\n", (ce - cs), (le - ls));
            if ((ce - cs) > (le - ls)) {
                le = ce;
                ls = cs;
            }
            cs = ce = s;
        }
        ce++;
        s++;
        if (cs)
            printf("cs %c, ce %c\n", *cs, *ce);
        if (ls)
            printf("ls %c, le %c\n", *ls, *le);
    }

    if (cs != NULL) {
        printf("3> %d %d\n\n", (ce - cs), (le - ls));
        if ((ce - cs) > (le - ls)) {
            le = ce;
            ls = cs;
            cs = ce = NULL;
        }
    }
    printf("ls %c, le %c\n", *ls, *le);
    return le - ls;
}
#endif
// int lengthOfLongestSubstring(char * s){

//     // s = "abcabcbb";
//     // s = "bbbbb";
//     // s = "dvdf";
//     // s = "abcdeabcde";   // 5
//     // s = "abcdeabcd";    // 5
//     // s = "abcdebcde";    // 5
//     // s = "abcdebcdefg";  // 6
//     // s = "abcdeefg";     // 5

//     // s = "abcdedcba";   // 5
//     // s = "abcdedfghijk";   // 8
//     s = "aab";

//     if (s == NULL)
//         return 0;

//     if (*s == '\0')
//         return 0;

//     char *ls = s, *le = s, *cs = s, *ce = s + 1, *cc;
//     char last = '\0';

//     while (*s != '\0') {
//         if (ce != NULL) {
//             cc = cs;
//             while (cc < ce) {
//                 if (*cc == *ce) {
//                     if ((ce - cs) > (le - ls)) {
//                         le = ce;
//                         ls = cs;
//                     }
//                     cs = cc + 1;
//                     break;
//                 }
//                 cc++;
//             }
//         }

//         if (*s != last) {
//             last = *s;
//         } else {
//             if ((ce - cs) > (le - ls)) {
//                 le = ce;
//                 ls = cs;
//             }
//             cs = ce = s;
//         }
//         ce++;
//         s++;
//     }

//     // if (cs != NULL) {
//     //     printf("3> %d %d\n\n", (ce - cs), (le - ls));
//     //     if ((ce - cs) > (le - ls)) {
//     //         le = ce;
//     //         ls = cs;
//     //         cs = ce = NULL;
//     //     }
//     // }
//     // printf("ls %c, le %c\n", *ls, *le);
//     return le - ls;
// }

#define STRING_BUF_SIZE        (256)
char str[STRING_BUF_SIZE];
char id[STRING_BUF_SIZE];
char time_str[STRING_BUF_SIZE];
char songname[STRING_BUF_SIZE];
char singer[STRING_BUF_SIZE];
char start[STRING_BUF_SIZE];
char yt_url[STRING_BUF_SIZE];

static long file_get_line_num(FILE *fp)
{
    long i = 0, pos;

    // Backup the current location of file position indicator
    pos = ftell(fp);

    while (fgets(str, STRING_BUF_SIZE, fp) != NULL) {
        i++;
    }
    // Restore the location of file position indicator
    fseek(fp, pos, SEEK_SET);

    return i;
}

static int file_songlist_to_wikitable(ALGO_FILE *s_file, ALGO_FILE *w_file)
{
    int ret = ALGO_ERROR_UNKNOWN;
    int l, r = 0;
    long line_num;
    char *sp, *ep, *tp;

    // Set the location of file position indicator to the beginning
    fseek(s_file->fp, 0, SEEK_SET);
    fseek(w_file->fp, 0, SEEK_SET);

    // Start to parse the file
    for (r = 0; r < 2; r++) {
        // Set the location of file position indicator to the beginning
        // Get line number of input file
        fseek(s_file->fp, 0, SEEK_SET);
        line_num = file_get_line_num(s_file->fp);

        if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
            ret = ALGO_ERROR_READ_FILE;
            goto exit;
        }
        sp = strchr(str, '\n');
        *sp = '\0';

        // Get id (v=tlsjhiZepx0)
        // https://www.youtube.com/watch?v=tlsjhiZepx0&t=241s
        sp = strchr(str, '=') + 1;
        ep = strchr(sp, '&');
        *ep++ = '\0';
        strcpy(id, sp);
        printf("id: %s\n", id);

        // Get start (t=241s)
        // https://www.youtube.com/watch?v=tlsjhiZepx0&t=241s
        sp = ep;
        sp = strchr(sp, '=') + 1;
        ep = strchr(sp, 's');
        *ep = '\0';
        strcpy(start, sp);
        printf("start: %s\n", start);

        // Get a new line
        if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
            ret = ALGO_ERROR_READ_FILE;
            goto exit;
        }
        sp = strchr(str, '\n');
        *sp = '\0';
        if (r == 0)
            fprintf(w_file->fp, "=== %s", str);
        if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
            ret = ALGO_ERROR_READ_FILE;
            goto exit;
        }
        sp = strchr(str, '\n');
        *sp = '\0';
        if (r == 0)
            fprintf(w_file->fp, "%s ===\n", str);

        if (r == 0) {
            fprintf(w_file->fp, "%s\n", "<mobileonly>");
            fprintf(w_file->fp, "%s\n", "{| class=\"wikitable\" style=\"margin: 1em auto 1em auto;\"");
        } else {
            fprintf(w_file->fp, "%s\n", "<nomobile>");
            fprintf(w_file->fp, "%s\n", "{| class=\"wikitable\" <!--style=\"margin: 1em auto 1em auto;\"-->");
        }

        // if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
        //     ret = ALGO_ERROR_READ_FILE;
        //     goto exit;
        // }
        // sp = strchr(str, '\n');
        // *sp = '\0';

        fprintf(w_file->fp, "|+\n");
        // if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
        //     ret = ALGO_ERROR_READ_FILE;
        //     goto exit;
        // }
        // sp = strchr(str, '\n');
        // *sp = '\0';
        // fprintf(w_file->fp, "%s\n", str);
        fprintf(w_file->fp, "|-\n");
        printf("id: %s\n", id);
        printf("start: %s\n", start);

        if (r == 0) {
            fprintf(w_file->fp, "| %s | {{#widget:Youtube|id=%s?start=%d}}\n", "style=\"width: 960px;\" colspan=\"5\"", id, atoi(start));
            fprintf(w_file->fp, "|-\n");
        } else {
            fprintf(w_file->fp, "| %s%d\" | {{#widget:Youtube|id=%s?start=%d}}\n", "style=\"width: 540px;\" rowspan=\"", line_num - 2, id, atoi(start));
        }
        fprintf(w_file->fp, "| style=\"width: 3em;\" | 曲目 || style=\"width: 10em;\" | 歌曲名稱 || style=\"width: 13em;\" | 原唱 || style=\"width: 5em;\" | 時間 || 備註\n");

        // return 0;
        for (l = 3; l < line_num; l++) {
            if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
                ret = ALGO_ERROR_READ_FILE;
                goto exit;
            }
            sp = strchr(str, '\n');
            if (sp != NULL)
                *sp = '\0';

            // find the first '.' character
            // Get index
            tp = str;
            sp = strchr(tp, '.');
            *sp++ = '\0';
            int index;
            if (*tp >= '0' && *tp <= '9')
                index = atoi(tp);
            else
                index = -1;


            printf("%d ", index);

            // Skip ' '
            while (*sp == ' ')
                sp++;
            memset(time_str, 0, STRING_BUF_SIZE);
            memcpy(time_str, sp, 7);
            tp = sp;
            sp = strchr(tp, ':');
            *sp++ = '\0';
            int time2  = atoi(tp);

            tp = sp;
            sp = strchr(tp, ':');
            *sp++ = '\0';
            time2  = time2 * 60 + atoi(tp);

            tp = sp;
            sp = strchr(tp, ' ');
            *sp++ = '\0';
            time2  = time2 * 60 + atoi(tp);
            printf("%d ", time2);

            while (*sp == ' ')
                sp++;

            // sp = strchr(sp, '\0');
            // sp++;
            // printf("%s", sp);

            // Get singer
            tp = sp;
            sp = strchr(sp, '/');
            sp++;
            while (*sp == ' ')
                sp++;
            strcpy(singer, sp);

            // Get song name
            sp = tp;
            sp = strchr(sp, '/');
            while (sp[-1] == ' ')
                sp--;

            *sp = '\0';
            strcpy(songname, tp);
            printf("%s %s\n", songname, singer);

            fprintf(w_file->fp, "|-\n");
            // fprintf(w_file->fp, "| %d || %s || %s || %s || \n", index, songname, singer);
            if (index != -1)
                fprintf(w_file->fp, "| %d || %s || %s || [https://www.youtube.com/watch?v=%s&t=%ds %s]\n"
                        , index, songname, singer, id, time2, time_str);
            else
                fprintf(w_file->fp, "| %s || %s || %s || [https://www.youtube.com/watch?v=%s&t=%ds %s]\n"
                        , "安可曲", songname, singer, id, time2, time_str);

        }
        fprintf(w_file->fp, "|}\n");
        if (r == 0)
            fprintf(w_file->fp, "%s\n", "</mobileonly>");
        else
            fprintf(w_file->fp, "%s\n", "</nomobile>");
    }

    ret = ALGO_SUCCESS;

exit:
    return ret;
}

ret_code songlist_to_wikitable(void)
{
    int i, ret = E_UNKNOWN, ferr;

    const char *yt_url = "";
    const char *file_name = "songlist.txt";
    ALGO_FILE s_file = {.fp = NULL, .file_name = "songlist.txt"};
    ALGO_FILE w_file = {.fp = NULL, .file_name = "wikitable.txt"};
    FILE *sfp, *wfp;

    if (file_open(&s_file, s_file.file_name, "r")) {
        goto exit;
    }

    if (file_open(&w_file, w_file.file_name, "w")) {
        goto exit;
    }

    file_songlist_to_wikitable(&s_file, &w_file);

    if (feof(s_file.fp)) {
        printf("feof\n");
    }

    ferr = ferror(s_file.fp);
    if (ferr) {
        printf("ferror: %d\n", ferr);
        perror("");
        clearerr(s_file.fp);
        goto exit;
    }

    if (file_close(&s_file)) {
        return ret;
    }

    if (file_close(&w_file)) {
        return ret;
    }

    ret = 0;

exit:
    if (s_file.fp) {
        file_close(&s_file);
    }

    if (w_file.fp) {
        file_close(&w_file);
    }

    return ret;
}

void c_test(void)
{
    printf("%s start >>>\n", __FUNCTION__);
    // // char s[] = "This is a pen";
    // char s[] = "The master branch at origin should be familiar to every Git user. Parallel to the master branch, another branch exists called develop.";
    // char *t = malloc(sizeof(s));

    // printf("sizeof(s): %d\n", sizeof(s));
    // memcpy(t, s, sizeof(s));
    // printd(123456789);
    // printf("\n");

    // printf("%s\n", t);
    // invert_sentence(t);
    // printf("%s\n", t);

    // char *s = "FFFFFFFFFFFFFFF11111111111111111111AAA";
    // printf("strongPasswordChecker: %d\n", strongPasswordChecker(s));

    // char *path = "/";
    // char *path = "/.";
    // char *path = "/a/./b/../../c/";
    // char *path = "/home/";
    // char *path = "/../";
    // char *path = "/home//foo";
    // char *path = "/a/../../b/../c//.//";
    // printf("path: %s, simplifyPath: %s\n", path, simplifyPath(path));

    // char *s;
    // s = "lee(t(c)o)de)";
    // s = "a)b(c)d";
    // s = "))((";
    // s = "())()(((";
    // printf("minRemoveToMakeValid: %s\n", minRemoveToMakeValid(s));

    // int nums[] = {2,7,11,15};
    // int numsSize = 4;
    // int target = 9;

    // int nums[] = {3,2,4};
    // int numsSize = 3;
    // int target = 6;

    // int returnSize;
    // int *r;
    // r = twoSum(nums, numsSize, target, &returnSize);
    // printf("r[0]=%d, r[1]=%d\n", r[0], r[1]);

    // int nums[] = {-1,0,1,2,-1,-4};
    // int numsSize = 6;
    // int nums[] = {0,0,0};
    // int numsSize = 3;
    // int nums[] = {-1,0,1,2,-1,-4,-2,-3,3,0,4};
    // int numsSize = 11;
    // int nums[] = {-4,-2,-2,-2,0,1,2,2,2,3,3,4,4,6,6};
    // int numsSize = 15;
    // int nums[] = {82597,-9243,62390,83030,-97960,-26521,-61011,83390,-38677,12333,75987,46091,83794,19355,-71037,-6242,-28801,324,1202,-90885,-2989,-95597,-34333,35528,5680,89093,-90606,50360,-29393,-27012,53313,65213,99818,-82405,-41661,-3333,-51952,72135,-1523,26377,74685,96992,92263,15929,5467,-99555,-43348,-41689,-60383,-3990,32165,65265,-72973,-58372,12741,-48568,-46596,72419,-1859,34153,62937,81310,-61823,-96770,-54944,8845,-91184,24208,-29078,31495,65258,14198,85395,70506,-40908,56740,-12228,-40072,32429,93001,68445,-73927,25731,-91859,-24150,10093,-60271,-81683,-18126,51055,48189,-6468,25057,81194,-58628,74042,66158,-14452,-49851,-43667,11092,39189,-17025,-79173,13606,83172,92647,-59741,19343,-26644,-57607,82908,-20655,1637,80060,98994,39331,-31274,-61523,91225,-72953,13211,-75116,-98421,-41571,-69074,99587,39345,42151,-2460,98236,15690,-52507,-95803,-48935,-46492,-45606,-79254,-99851,52533,73486,39948,-7240,71815,-585,-96252,90990,-93815,93340,-71848,58733,-14859,-83082,-75794,-82082,-24871,-15206,91207,-56469,-93618,67131,-8682,75719,87429,-98757,-7535,-24890,-94160,85003,33928,75538,97456,-66424,-60074,-8527,-28697,-22308,2246,-70134,-82319,-10184,87081,-34949,-28645,-47352,-83966,-60418,-15293,-53067,-25921,55172,75064,95859,48049,34311,-86931,-38586,33686,-36714,96922,76713,-22165,-80585,-34503,-44516,39217,-28457,47227,-94036,43457,24626,-87359,26898,-70819,30528,-32397,-69486,84912,-1187,-98986,-32958,4280,-79129,-65604,9344,58964,50584,71128,-55480,24986,15086,-62360,-42977,-49482,-77256,-36895,-74818,20,3063,-49426,28152,-97329,6086,86035,-88743,35241,44249,19927,-10660,89404,24179,-26621,-6511,57745,-28750,96340,-97160,-97822,-49979,52307,79462,94273,-24808,77104,9255,-83057,77655,21361,55956,-9096,48599,-40490,-55107,2689,29608,20497,66834,-34678,23553,-81400,-66630,-96321,-34499,-12957,-20564,25610,-4322,-58462,20801,53700,71527,24669,-54534,57879,-3221,33636,3900,97832,-27688,-98715,5992,24520,-55401,-57613,-69926,57377,-77610,20123,52174,860,60429,-91994,-62403,-6218,-90610,-37263,-15052,62069,-96465,44254,89892,-3406,19121,-41842,-87783,-64125,-56120,73904,-22797,-58118,-4866,5356,75318,46119,21276,-19246,-9241,-97425,57333,-15802,93149,25689,-5532,95716,39209,-87672,-29470,-16324,-15331,27632,-39454,56530,-16000,29853,46475,78242,-46602,83192,-73440,-15816,50964,-36601,89758,38375,-40007,-36675,-94030,67576,46811,-64919,45595,76530,40398,35845,41791,67697,-30439,-82944,63115,33447,-36046,-50122,-34789,43003,-78947,-38763,-89210,32756,-20389,-31358,-90526,-81607,88741,86643,98422,47389,-75189,13091,95993,-15501,94260,-25584,-1483,-67261,-70753,25160,89614,-90620,-48542,83889,-12388,-9642,-37043,-67663,28794,-8801,13621,12241,55379,84290,21692,-95906,-85617,-17341,-63767,80183,-4942,-51478,30997,-13658,8838,17452,-82869,-39897,68449,31964,98158,-49489,62283,-62209,-92792,-59342,55146,-38533,20496,62667,62593,36095,-12470,5453,-50451,74716,-17902,3302,-16760,-71642,-34819,96459,-72860,21638,47342,-69897,-40180,44466,76496,84659,13848,-91600,-90887,-63742,-2156,-84981,-99280,94326,-33854,92029,-50811,98711,-36459,-75555,79110,-88164,-97397,-84217,97457,64387,30513,-53190,-83215,252,2344,-27177,-92945,-89010,82662,-11670,86069,53417,42702,97082,3695,-14530,-46334,17910,77999,28009,-12374,15498,-46941,97088,-35030,95040,92095,-59469,-24761,46491,67357,-66658,37446,-65130,-50416,99197,30925,27308,54122,-44719,12582,-99525,-38446,-69050,-22352,94757,-56062,33684,-40199,-46399,96842,-50881,-22380,-65021,40582,53623,-76034,77018,-97074,-84838,-22953,-74205,79715,-33920,-35794,-91369,73421,-82492,63680,-14915,-33295,37145,76852,-69442,60125,-74166,74308,-1900,-30195,-16267,-60781,-27760,5852,38917,25742,-3765,49097,-63541,98612,-92865,-30248,9612,-8798,53262,95781,-42278,-36529,7252,-27394,-5021,59178,80934,-48480,-75131,-54439,-19145,-48140,98457,-6601,-51616,-89730,78028,32083,-48904,16822,-81153,-8832,48720,-80728,-45133,-86647,-4259,-40453,2590,28613,50523,-4105,-27790,-74579,-17223,63721,33489,-47921,97628,-97691,-14782,-65644,18008,-93651,-71266,80990,-76732,-47104,35368,28632,59818,-86269,-89753,34557,-92230,-5933,-3487,-73557,-13174,-43981,-43630,-55171,30254,-83710,-99583,-13500,71787,5017,-25117,-78586,86941,-3251,-23867,-36315,75973,86272,-45575,77462,-98836,-10859,70168,-32971,-38739,-12761,93410,14014,-30706,-77356,-85965,-62316,63918,-59914,-64088,1591,-10957,38004,15129,-83602,-51791,34381,-89382,-26056,8942,5465,71458,-73805,-87445,-19921,-80784,69150,-34168,28301,-68955,18041,6059,82342,9947,39795,44047,-57313,48569,81936,-2863,-80932,32976,-86454,-84207,33033,32867,9104,-16580,-25727,80157,-70169,53741,86522,84651,68480,84018,61932,7332,-61322,-69663,76370,41206,12326,-34689,17016,82975,-23386,39417,72793,44774,-96259,3213,79952,29265,-61492,-49337,14162,65886,3342,-41622,-62659,-90402,-24751,88511,54739,-21383,-40161,-96610,-24944,-602,-76842,-21856,69964,43994,-15121,-85530,12718,13170,-13547,69222,62417,-75305,-81446,-38786,-52075,-23110,97681,-82800,-53178,11474,35857,94197,-58148,-23689,32506,92154,-64536,-73930,-77138,97446,-83459,70963,22452,68472,-3728,-25059,-49405,95129,-6167,12808,99918,30113,-12641,-26665,86362,-33505,50661,26714,33701,89012,-91540,40517,-12716,-57185,-87230,29914,-59560,13200,-72723,58272,23913,-45586,-96593,-26265,-2141,31087,81399,92511,-34049,20577,2803,26003,8940,42117,40887,-82715,38269,40969,-50022,72088,21291,-67280,-16523,90535,18669,94342,-39568,-88080,-99486,-20716,23108,-28037,63342,36863,-29420,-44016,75135,73415,16059,-4899,86893,43136,-7041,33483,-67612,25327,40830,6184,61805,4247,81119,-22854,-26104,-63466,63093,-63685,60369,51023,51644,-16350,74438,-83514,99083,10079,-58451,-79621,48471,67131,-86940,99093,11855,-22272,-67683,-44371,9541,18123,37766,-70922,80385,-57513,-76021,-47890,36154,72935,84387,-92681,-88303,-7810,59902,-90,-64704,-28396,-66403,8860,13343,33882,85680,7228,28160,-14003,54369,-58893,92606,-63492,-10101,64714,58486,29948,-44679,-22763,10151,-56695,4031,-18242,-36232,86168,-14263,9883,47124,47271,92761,-24958,-73263,-79661,-69147,-18874,29546,-92588,-85771,26451,-86650,-43306,-59094,-47492,-34821,-91763,-47670,33537,22843,67417,-759,92159,63075,94065,-26988,55276,65903,30414,-67129,-99508,-83092,-91493,-50426,14349,-83216,-76090,32742,-5306,-93310,-60750,-60620,-45484,-21108,-58341,-28048,-52803,69735,78906,81649,32565,-86804,-83202,-65688,-1760,89707,93322,-72750,84134,71900,-37720,19450,-78018,22001,-23604,26276,-21498,65892,-72117,-89834,-23867,55817,-77963,42518,93123,-83916,63260,-2243,-97108,85442,-36775,17984,-58810,99664,-19082,93075,-69329,87061,79713,16296,70996,13483,-74582,49900,-27669,-40562,1209,-20572,34660,83193,75579,7344,64925,88361,60969,3114,44611,-27445,53049,-16085,-92851,-53306,13859,-33532,86622,-75666,-18159,-98256,51875,-42251,-27977,-18080,23772,38160,41779,9147,94175,99905,-85755,62535,-88412,-52038,-68171,93255,-44684,-11242,-104,31796,62346,-54931,-55790,-70032,46221,56541,-91947,90592,93503,4071,20646,4856,-63598,15396,-50708,32138,-85164,38528,-89959,53852,57915,-42421,-88916,-75072,67030,-29066,49542,-71591,61708,-53985,-43051,28483,46991,-83216,80991,-46254,-48716,39356,-8270,-47763,-34410,874,-1186,-7049,28846,11276,21960,-13304,-11433,-4913,55754,79616,70423,-27523,64803,49277,14906,-97401,-92390,91075,70736,21971,-3303,55333,-93996,76538,54603,-75899,98801,46887,35041,48302,-52318,55439,24574,14079,-24889,83440,14961,34312,-89260,-22293,-81271,-2586,-71059,-10640,-93095,-5453,-70041,66543,74012,-11662,-52477,-37597,-70919,92971,-17452,-67306,-80418,7225,-89296,24296,86547,37154,-10696,74436,-63959,58860,33590,-88925,-97814,-83664,85484,-8385,-50879,57729,-74728,-87852,-15524,-91120,22062,28134,80917,32026,49707,-54252,-44319,-35139,13777,44660,85274,25043,58781,-89035,-76274,6364,-63625,72855,43242,-35033,12820,-27460,77372,-47578,-61162,-70758,-1343,-4159,64935,56024,-2151,43770,19758,-30186,-86040,24666,-62332,-67542,73180,-25821,-27826,-45504,-36858,-12041,20017,-24066,-56625,-52097,-47239,-90694,8959,7712,-14258,-5860,55349,61808,-4423,-93703,64681,-98641,-25222,46999,-83831,-54714,19997,-68477,66073,51801,-66491,52061,-52866,79907,-39736,-68331,68937,91464,98892,910,93501,31295,-85873,27036,-57340,50412,21,-2445,29471,71317,82093,-94823,-54458,-97410,39560,-7628,66452,39701,54029,37906,46773,58296,60370,-61090,85501,-86874,71443,-72702,-72047,14848,34102,77975,-66294,-36576,31349,52493,-70833,-80287,94435,39745,-98291,84524,-18942,10236,93448,50846,94023,-6939,47999,14740,30165,81048,84935,-19177,-13594,32289,62628,-90612,-542,-66627,64255,71199,-83841,-82943,-73885,8623,-67214,-9474,-35249,62254,-14087,-90969,21515,-83303,94377,-91619,19956,-98810,96727,-91939,29119,-85473,-82153,-69008,44850,74299,-76459,-86464,8315,-49912,-28665,59052,-69708,76024,-92738,50098,18683,-91438,18096,-19335,35659,91826,15779,-73070,67873,-12458,-71440,-46721,54856,97212,-81875,35805,36952,68498,81627,-34231,81712,27100,-9741,-82612,18766,-36392,2759,41728,69743,26825,48355,-17790,17165,56558,3295,-24375,55669,-16109,24079,73414,48990,-11931,-78214,90745,19878,35673,-15317,-89086,94675,-92513,88410,-93248,-19475,-74041,-19165,32329,-26266,-46828,-18747,45328,8990,-78219,-25874,-74801,-44956,-54577,-29756,-99822,-35731,-18348,-68915,-83518,-53451,95471,-2954,-13706,-8763,-21642,-37210,16814,-60070,-42743,27697,-36333,-42362,11576,85742,-82536,68767,-56103,-63012,71396,-78464,-68101,-15917,-11113,-3596,77626,-60191,-30585,-73584,6214,-84303,18403,23618,-15619,-89755,-59515,-59103,-74308,-63725,-29364,-52376,-96130,70894,-12609,50845,-2314,42264,-70825,64481,55752,4460,-68603,-88701,4713,-50441,-51333,-77907,97412,-66616,-49430,60489,-85262,-97621,-18980,44727,-69321,-57730,66287,-92566,-64427,-14270,11515,-92612,-87645,61557,24197,-81923,-39831,-10301,-23640,-76219,-68025,92761,-76493,68554,-77734,-95620,-11753,-51700,98234,-68544,-61838,29467,46603,-18221,-35441,74537,40327,-58293,75755,-57301,-7532,-94163,18179,-14388,-22258,-46417,-48285,18242,-77551,82620,250,-20060,-79568,-77259,82052,-98897,-75464,48773,-79040,-11293,45941,-67876,-69204,-46477,-46107,792,60546,-34573,-12879,-94562,20356,-48004,-62429,96242,40594,2099,99494,25724,-39394,-2388,-18563,-56510,-83570,-29214,3015,74454,74197,76678,-46597,60630,-76093,37578,-82045,-24077,62082,-87787,-74936,58687,12200,-98952,70155,-77370,21710,-84625,-60556,-84128,925,65474,-15741,-94619,88377,89334,44749,22002,-45750,-93081,-14600,-83447,46691,85040,-66447,-80085,56308,44310,24979,-29694,57991,4675,-71273,-44508,13615,-54710,23552,-78253,-34637,50497,68706,81543,-88408,-21405,6001,-33834,-21570,-46692,-25344,20310,71258,-97680,11721,59977,59247,-48949,98955,-50276,-80844,-27935,-76102,55858,-33492,40680,66691,-33188,8284,64893,-7528,6019,-85523,8434,-64366,-56663,26862,30008,-7611,-12179,-70076,21426,-11261,-36864,-61937,-59677,929,-21052,3848,-20888,-16065,98995,-32293,-86121,-54564,77831,68602,74977,31658,40699,29755,98424,80358,-69337,26339,13213,-46016,-18331,64713,-46883,-58451,-70024,-92393,-4088,70628,-51185,71164,-75791,-1636,-29102,-16929,-87650,-84589,-24229,-42137,-15653,94825,13042,88499,-47100,-90358,-7180,29754,-65727,-42659,-85560,-9037,-52459,20997,-47425,17318,21122,20472,-23037,65216,-63625,-7877,-91907,24100,-72516,22903,-85247,-8938,73878,54953,87480,-31466,-99524,35369,-78376,89984,-15982,94045,-7269,23319,-80456,-37653,-76756,2909,81936,54958,-12393,60560,-84664,-82413,66941,-26573,-97532,64460,18593,-85789,-38820,-92575,-43663,-89435,83272,-50585,13616,-71541,-53156,727,-27644,16538,34049,57745,34348,35009,16634,-18791,23271,-63844,95817,21781,16590,59669,15966,-6864,48050,-36143,97427,-59390,96931,78939,-1958,50777,43338,-51149,39235,-27054,-43492,67457,-83616,37179,10390,85818,2391,73635,87579,-49127,-81264,-79023,-81590,53554,-74972,-83940,-13726,-39095,29174,78072,76104,47778,25797,-29515,-6493,-92793,22481,-36197,-65560,42342,15750,97556,99634,-56048,-35688,13501,63969,-74291,50911,39225,93702,-3490,-59461,-30105,-46761,-80113,92906,-68487,50742,36152,-90240,-83631,24597,-50566,-15477,18470,77038,40223,-80364,-98676,70957,-63647,99537,13041,31679,86631,37633,-16866,13686,-71565,21652,-46053,-80578,-61382,68487,-6417,4656,20811,67013,-30868,-11219,46,74944,14627,56965,42275,-52480,52162,-84883,-52579,-90331,92792,42184,-73422,-58440,65308,-25069,5475,-57996,59557,-17561,2826,-56939,14996,-94855,-53707,99159,43645,-67719,-1331,21412,41704,31612,32622,1919,-69333,-69828,22422,-78842,57896,-17363,27979,-76897,35008,46482,-75289,65799,20057,7170,41326,-76069,90840,-81253,-50749,3649,-42315,45238,-33924,62101,96906,58884,-7617,-28689,-66578,62458,50876,-57553,6739,41014,-64040,-34916,37940,13048,-97478,-11318,-89440,-31933,-40357,-59737,-76718,-14104,-31774,28001,4103,41702,-25120,-31654,63085,-3642,84870,-83896,-76422,-61520,12900,88678,85547,33132,-88627,52820,63915,-27472,78867,-51439,33005,-23447,-3271,-39308,39726,-74260,-31874,-36893,93656,910,-98362,60450,-88048,99308,13947,83996,-90415,-35117,70858,-55332,-31721,97528,82982,-86218,6822,25227,36946,97077,-4257,-41526,56795,89870,75860,-70802,21779,14184,-16511,-89156,-31422,71470,69600,-78498,74079,-19410,40311,28501,26397,-67574,-32518,68510,38615,19355,-6088,-97159,-29255,-92523,3023,-42536,-88681,64255,41206,44119,52208,39522,-52108,91276,-70514,83436,63289,-79741,9623,99559,12642,85950,83735,-21156,-67208,98088,-7341,-27763,-30048,-44099,-14866,-45504,-91704,19369,13700,10481,-49344,-85686,33994,19672,36028,60842,66564,-24919,33950,-93616,-47430,-35391,-28279,56806,74690,39284,-96683,-7642,-75232,37657,-14531,-86870,-9274,-26173,98640,88652,64257,46457,37814,-19370,9337,-22556,-41525,39105,-28719,51611,-93252,98044,-90996,21710,-47605,-64259,-32727,53611,-31918,-3555,33316,-66472,21274,-37731,-2919,15016,48779,-88868,1897,41728,46344,-89667,37848,68092,-44011,85354,-43776,38739,-31423,-66330,65167,-22016,59405,34328,-60042,87660,-67698,-59174,-1408,-46809,-43485,-88807,-60489,13974,22319,55836,-62995,-37375,-4185,32687,-36551,-75237,58280,26942,-73756,71756,78775,-40573,14367,-71622,-77338,24112,23414,-7679,-51721,87492,85066,-21612,57045,10673,-96836,52461,-62218,-9310,65862,-22748,89906,-96987,-98698,26956,-43428,46141,47456,28095,55952,67323,-36455,-60202,-43302,-82932,42020,77036,10142,60406,70331,63836,58850,-66752,52109,21395,-10238,-98647,-41962,27778,69060,98535,-28680,-52263,-56679,66103,-42426,27203,80021,10153,58678,36398,63112,34911,20515,62082,-15659,-40785,27054,43767,-20289,65838,-6954,-60228,-72226,52236,-35464,25209,-15462,-79617,-41668,-84083,62404,-69062,18913,46545,20757,13805,24717,-18461,-47009,-25779,68834,64824,34473,39576,31570,14861,-15114,-41233,95509,68232,67846,84902,-83060,17642,-18422,73688,77671,-26930,64484,-99637,73875,6428,21034,-73471,19664,-68031,15922,-27028,48137,54955,-82793,-41144,-10218,-24921,-28299,-2288,68518,-54452,15686,-41814,66165,-72207,-61986,80020,50544,-99500,16244,78998,40989,14525,-56061,-24692,-94790,21111,37296,-90794,72100,70550,-31757,17708,-74290,61910,78039,-78629,-25033,73172,-91953,10052,64502,99585,-1741,90324,-73723,68942,28149,30218,24422,16659,10710,-62594,94249,96588,46192,34251,73500,-65995,-81168,41412,-98724,-63710,-54696,-52407,19746,45869,27821,-94866,-76705,-13417,-61995,-71560,43450,67384,-8838,-80293,-28937,23330,-89694,-40586,46918,80429,-5475,78013,25309,-34162,37236,-77577,86744,26281,-29033,-91813,35347,13033,-13631,-24459,3325,-71078,-75359,81311,19700,47678,-74680,-84113,45192,35502,37675,19553,76522,-51098,-18211,89717,4508,-82946,27749,85995,89912,-53678,-64727,-14778,32075,-63412,-40524,86440,-2707,-36821,63850,-30883,67294,-99468,-23708,34932,34386,98899,29239,-23385,5897,54882,98660,49098,70275,17718,88533,52161,63340,50061,-89457,19491,-99156,24873,-17008,64610,-55543,50495,17056,-10400,-56678,-29073,-42960,-76418,98562,-88104,-96255,10159,-90724,54011,12052,45871,-90933,-69420,67039,37202,78051,-52197,-40278,-58425,65414,-23394,-1415,6912,-53447,7352,17307,-78147,63727,98905,55412,-57658,-32884,-44878,22755,39730,3638,35111,39777,74193,38736,-11829,-61188,-92757,55946,-71232,-63032,-83947,39147,-96684,-99233,25131,-32197,24406,-55428,-61941,25874,-69453,64483,-19644,-68441,12783,87338,-48676,66451,-447,-61590,50932,-11270,29035,65698,-63544,10029,80499,-9461,86368,91365,-81810,-71914,-52056,-13782,44240,-30093,-2437,24007,67581,-17365,-69164,-8420,-69289,-29370,48010,90439,13141,69243,50668,39328,61731,78266,-81313,17921,-38196,55261,9948,-24970,75712,-72106,28696,7461,31621,61047,51476,56512,11839,-96916,-82739,28924,-99927,58449,37280,69357,11219,-32119,-62050,-48745,-83486,-52376,42668,82659,68882,38773,46269,-96005,97630,25009,-2951,-67811,99801,81587,-79793,-18547,-83086,69512,33127,-92145,-88497,47703,59527,1909,88785,-88882,69188,-46131,-5589,-15086,36255,-53238,-33009,82664,53901,35939,-42946,-25571,33298,69291,53199,74746,-40127,-39050,91033,51717,-98048,87240,36172,65453,-94425,-63694,-30027,59004,88660,3649,-20267,-52565,-67321,34037,4320,91515,-56753,60115,27134,68617,-61395,-26503,-98929,-8849,-63318,10709,-16151,61905,-95785,5262,23670,-25277,90206,-19391,45735,37208,-31992,-92450,18516,-90452,-58870,-58602,93383,14333,17994,82411,-54126,-32576,35440,-60526,-78764,-25069,-9022,-394,92186,-38057,55328,-61569,67780,77169,19546,-92664,-94948,44484,-13439,83529,27518,-48333,72998,38342,-90553,-98578,-76906,81515,-16464,78439,92529,35225,-39968,-10130,-7845,-32245,-74955,-74996,67731,-13897,-82493,33407,93619,59560,-24404,-57553,19486,-45341,34098,-24978,-33612,79058,71847,76713,-95422,6421,-96075,-59130,-28976,-16922,-62203,69970,68331,21874,40551,89650,51908,58181,66480,-68177,34323,-3046,-49656,-59758,43564,-10960,-30796,15473,-20216,46085,-85355,41515,-30669,-87498,57711,56067,63199,-83805,62042,91213,-14606,4394,-562,74913,10406,96810,-61595,32564,31640,-9732,42058,98052,-7908,-72330,1558,-80301,34878,32900,3939,-8824,88316,20937,21566,-3218,-66080,-31620,86859,54289,90476,-42889,-15016,-18838,75456,30159,-67101,42328,-92703,85850,-5475,23470,-80806,68206,17764,88235,46421,-41578,74005,-81142,80545,20868,-1560,64017,83784,68863,-97516,-13016,-72223,79630,-55692,82255,88467,28007,-34686,-69049,-41677,88535,-8217,68060,-51280,28971,49088,49235,26905,-81117,-44888,40623,74337,-24662,97476,79542,-72082,-35093,98175,-61761,-68169,59697,-62542,-72965,59883,-64026,-37656,-92392,-12113,-73495,98258,68379,-21545,64607,-70957,-92254,-97460,-63436,-8853,-19357,-51965,-76582,12687,-49712,45413,-60043,33496,31539,-57347,41837,67280,-68813,52088,-13155,-86430,-15239,-45030,96041,18749,-23992,46048,35243,-79450,85425,-58524,88781,-39454,53073,-48864,-82289,39086,82540,-11555,25014,-5431,-39585,-89526,2705,31953,-81611,36985,-56022,68684,-27101,11422,64655,-26965,-63081,-13840,-91003,-78147,-8966,41488,1988,99021,-61575,-47060,65260,-23844,-21781,-91865,-19607,44808,2890,63692,-88663,-58272,15970,-65195,-45416,-48444,-78226,-65332,-24568,42833,-1806,-71595,80002,-52250,30952,48452,-90106,31015,-22073,62339,63318,78391,28699,77900,-4026,-76870,-45943,33665,9174,-84360,-22684,-16832,-67949,-38077,-38987,-32847,51443,-53580,-13505,9344,-92337,26585,70458,-52764,-67471,-68411,-1119,-2072,-93476,67981,40887,-89304,-12235,41488,1454,5355,-34855,-72080,24514,-58305,3340,34331,8731,77451,-64983,-57876,82874,62481,-32754,-39902,22451,-79095,-23904,78409,-7418,77916};
    // int numsSize = 3000;
    // int returnSize;
    // int **rr;
    // int *returnColumnSizes;
    // rr = threeSum(nums, numsSize, &returnSize, &returnColumnSizes);


    // int nums[] = {1,0,-1,0,-2,2};
    // int target = 0;

    // int nums[] = {2,2,2,2,2};
    // int target = 8;

    // int nums[] = {-2,-1,-1,1,1,2,2};
    // int target = 0;
    // int nums[] = {1,-2,-5,-4,-3,3,3,5};
    // int target = -11;
    // int nums[] = {-489,-487,-471,-464,-451,-421,-414,-405,-396,-355,-354,-350,-336,-335,-334,-307,-298,-296,-295,-287,-267,-256,-247,-231,-170,-130,-120,-109,-96,-80,-78,-71,-63,-56,-44,-43,-13,2,9,18,31,36,39,43,49,49,50,61,68,73,99,107,112,113,120,121,173,180,185,190,203,210,233,246,258,296,319,340,345,370,371,378,395,418,436,444,447,451,460,485};
    // int target = 2926;
    // int numsSize = sizeof(nums) / sizeof(*nums);
    // int returnSize;
    // int **rr;
    // int *returnColumnSizes;

    // rr = fourSum(nums, numsSize, target, &returnSize, &returnColumnSizes);

    // printf("returnSize %d\n", returnSize);
    // for (int i = 0; i < (returnSize > 100 ? 100 : returnSize); i++) {
    //     printf("rr %d: r[0]=%d, r[1]=%d, r[2]=%d, r[3]=%d\n",
    //             i, rr[i][0], rr[i][1], rr[i][2], rr[i][3]);
    // }

    // printf("len: %d\n", lengthOfLongestSubstring(0));
    songlist_to_wikitable();
    printf("%s end <<<\n", __FUNCTION__);
}
