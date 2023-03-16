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

static int file_print_page_navigation_mobileonly(ALGO_FILE *w_file, int header)
{
    if (header)
        fprintf(w_file->fp, "== 頁面導覽 ==\n");
    fprintf(w_file->fp, "<mobileonly>\n");
    fprintf(w_file->fp, "{| class=\"wikitable\"\n");
    fprintf(w_file->fp, "|+\n");
    fprintf(w_file->fp, "!首頁 ▶ [[首頁]]\n");
    fprintf(w_file->fp, "!涅默介紹 ▶ [[涅默Nemesis]]\n");
    fprintf(w_file->fp, "|-\n");
    fprintf(w_file->fp, "!精華列表 ▶ [[精華影片]]\n");
    fprintf(w_file->fp, "!歌回列表 ▶ [[歌回]]\n");
    fprintf(w_file->fp, "|-\n");
    fprintf(w_file->fp, "!直播記錄 ▶ [[直播記錄]]\n");
    fprintf(w_file->fp, "!涅默語音 ▶ [[涅默語音庫]]\n");
    fprintf(w_file->fp, "|-\n");
    fprintf(w_file->fp, "!涅默畫廊 ▶ [[畫廊]]\n");
    fprintf(w_file->fp, "!編輯手冊 ▶ [[編輯導覽]]\n");
    fprintf(w_file->fp, "|}\n");
    fprintf(w_file->fp, "</mobileonly>\n");
}

static int file_print_page_navigation_nomobile(ALGO_FILE *w_file, int header)
{
    if (header)
        fprintf(w_file->fp, "== 頁面導覽 ==\n");
    fprintf(w_file->fp, "<nomobile>\n");
    fprintf(w_file->fp, "{| class=\"wikitable\"\n");
    fprintf(w_file->fp, "|+\n");
    fprintf(w_file->fp, "!首頁 ▶ [[首頁]]\n");
    fprintf(w_file->fp, "!涅默介紹 ▶ [[涅默Nemesis]]\n");
    fprintf(w_file->fp, "!精華影片 ▶ [[精華影片]]\n");
    fprintf(w_file->fp, "!歌回列表 ▶ [[歌回]]\n");
    fprintf(w_file->fp, "!直播記錄 ▶ [[直播記錄]]\n");
    fprintf(w_file->fp, "!涅默語音 ▶ [[涅默語音庫]]\n");
    fprintf(w_file->fp, "!涅默畫廊 ▶ [[畫廊]]\n");
    fprintf(w_file->fp, "!編輯手冊 ▶ [[編輯導覽]]\n");
    fprintf(w_file->fp, "|}\n");
    fprintf(w_file->fp, "</nomobile>\n");
}

static char _file_name[STRING_BUF_SIZE];
static char _source_url[STRING_BUF_SIZE];
static char _title[STRING_BUF_SIZE];
static char _twitter_id[STRING_BUF_SIZE];
static char _twitter_url[STRING_BUF_SIZE];

static int file_csv_to_gallerytable(ALGO_FILE *s_file, ALGO_FILE *w_file)
{
    int ret = ALGO_ERROR_UNKNOWN;
    int r, l, line_num;
    char *sp, *ep;

    // Set the location of file position indicator to the beginning
    fseek(s_file->fp, 0, SEEK_SET);
    fseek(w_file->fp, 0, SEEK_SET);

    fprintf(w_file->fp, "== 主題標籤/Hashtag ==\n");
    fprintf(w_file->fp, "請參見Twitter hashtag: [https://twitter.com/search?q=%%23NemomoArt #NemomoArt]\n\n");
    fprintf(w_file->fp, "== 畫廊 ==\n");

    // Start to parse the file
    for (r = 0; r < 2; r++) {
        fseek(s_file->fp, 0, SEEK_SET);
        line_num = file_get_line_num(s_file->fp);

        printf("line_num: %d\n", line_num);

        if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
            ret = ALGO_ERROR_READ_FILE;
            goto exit;
        }
        sp = strchr(str, '\n');
        *sp = '\0';

        if (r == 0) {
            fprintf(w_file->fp, "<mobileonly>\n");
            fprintf(w_file->fp, "<gallery widths=240px heights=160px mode=\"packed-hover\">\n");
        } else {
            fprintf(w_file->fp, "<nomobile>\n");
            fprintf(w_file->fp, "<gallery widths=480px heights=320px  mode=\"packed-hover\">\n");
        }

        for (l = 1; l < line_num; l++) {
            if (fgets(str, STRING_BUF_SIZE, s_file->fp) == NULL) {
                ret = ALGO_ERROR_READ_FILE;
                goto exit;
            }
            sp = str;
            ep = strchr(sp, '\n');
            *ep = '\0';

            // Get file name
            ep = strchr(sp, ',');
            *ep++ = '\0';
            strcpy(_file_name, sp);

            // Get source url
            sp = ep;
            ep = strchr(sp, ',');
            *ep++ = '\0';
            strcpy(_source_url, sp);

            sp = ep;
            if (*sp == ',')
                fprintf(w_file->fp, "File:%s|%s\n", _file_name, _source_url);
            else {
                // Get title
                ep = strchr(sp, ',');
                *ep++ = '\0';
                strcpy(_title, sp);
                // Get twitter id
                sp = ep;
                ep = strchr(sp, ',');
                *ep++ = '\0';
                strcpy(_twitter_id, sp);
                // Get twitter url
                strcpy(_twitter_url, ep);
                fprintf(w_file->fp, "File:%s|%s<br>%s: [%s %s]\n",
                    _file_name, _source_url,
                    _title, _twitter_url, _twitter_id);
            }
        }

        fprintf(w_file->fp, "</gallery>\n");
        if (r == 0)
            fprintf(w_file->fp, "</mobileonly>\n");
        else
            fprintf(w_file->fp, "</nomobile>\n\n");
    }

    file_print_page_navigation_mobileonly(w_file, 1);
    file_print_page_navigation_nomobile(w_file, 0);

    ret = ALGO_SUCCESS;

exit:
    return ret;
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

        fprintf(w_file->fp, "|+\n");
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

ret_code csv_to_gallerytable(void)
{
    int i, ret = E_UNKNOWN, ferr;

    ALGO_FILE s_file = {.fp = NULL, .file_name = "imagefile.csv"};
    ALGO_FILE w_file = {.fp = NULL, .file_name = "gallerytable.txt"};
    FILE *sfp, *wfp;

    if (file_open(&s_file, s_file.file_name, "r"))
        goto exit;

    if (file_open(&w_file, w_file.file_name, "w"))
        goto exit;

    file_csv_to_gallerytable(&s_file, &w_file);

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

    if (file_close(&s_file))
        return ret;

    if (file_close(&w_file))
        return ret;


    ret = 0;
exit:
    if (s_file.fp)
        file_close(&s_file);

    if (w_file.fp)
        file_close(&w_file);

    return ret;
}

ret_code songlist_to_wikitable(void)
{
    int i, ret = E_UNKNOWN, ferr;

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

#define RANDOM_ARRAY_SIZE_MAX   (9999)

typedef void (*alloc_func_t)(void *nums, int size);
typedef void (*init_func_t)(void *nums, int size);

void *generate_generic_random_array(
    void **nums, int *numsSize, int max,
    alloc_func_t alloc,
    init_func_t init)
{
    srand(time(0));
    *numsSize = rand() % max;
    alloc(*nums, *numsSize);
    init(*nums, *numsSize);
    return *nums;
}

int *generate_dynamic_random_array_int(int **nums, int *numsSize, int max)
{
    srand(time(0));
    *numsSize = rand() % max;
    *nums = (int *)malloc(sizeof(int) * (*numsSize));
    for (int i = 0; i < *numsSize; i++)
        (*nums)[i] = rand();
    return *nums;
}

int *generate_fix_random_array_int(int **nums, int numsSize)
{
    srand(time(0));
    *nums = (int *)malloc(sizeof(int) * (numsSize));
    for (int i = 0; i < numsSize; i++)
        (*nums)[i] = rand();
    return *nums;
}

/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int **threeSum2(int *nums, int numsSize, int *returnSize, int **returnColumnSizes)
{
    *returnSize = 0;
    if (nums == NULL || numsSize < 3)
        return NULL;

    int memSize = 2;
    int **res = (int **)malloc(sizeof(int *) * memSize);
    (*returnColumnSizes) = (int *)malloc(sizeof(int) * memSize);

    heap_sort_int(nums, 0, numsSize);

    int i, left, right, sum, temp;

    for (i = 0; i < numsSize - 2; i++) {
        if (nums[i] > 0 || nums[i] + nums[i + 1] + nums[i + 2] > 0)
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
                res[*returnSize] = (int *)malloc(sizeof(int) * 3);
                res[*returnSize][0] = nums[i];
                res[*returnSize][1] = nums[left++];
                res[*returnSize][2] = nums[right--];
                (*returnColumnSizes)[*returnSize] = 3;
                (*returnSize)++;

                while (left < right && (nums[left - 1]) == nums[left])
                    left++;

                while (right > left && (nums[right + 1] == nums[right]))
                    right--;

                if ((*returnSize) == memSize) {
                    memSize *= 2;
                    (*returnColumnSizes) = realloc((*returnColumnSizes), sizeof(int) * memSize);
                    res = realloc(res, sizeof(int *) * memSize);
                }
            }
        }

        return res;
    }
}

int threeSumClosest(int* nums, int numsSize, int target)
{
    heap_sort_int(nums, NULL, numsSize);

    int sum = target, minDiffValue = INT_MAX, diff;
    for (int i = 0; i < numsSize - 1; i++) {
        int left = i + 1;
        int right = numsSize - 1;
        int v1 = nums[i];
        int v2 = nums[left];
        int v3 = nums[right];
        while (left < right) {
            int temp = v1 + v2 + v3;

            if (target > temp) {
                diff = target - temp;
                v2 = nums[++left];
            } else if (target < temp) {
                diff = temp - target;
                v3 = nums[--right];
            }

            if (minDiffValue > diff) {
                if (diff == 0)
                    return temp;
                sum = temp;
                minDiffValue = diff;
            }
        }
    }

    return sum;
}

int threeSumClosest2(int* nums, int numsSize, int target)
{
    heap_sort_int(nums, NULL, numsSize);

    int sum = target, minDiffValue = INT_MAX;
    for (int i = 0; i < numsSize - 2; i++) {
        int left = i + 1;
        int right = numsSize - 1;
        int v1 = nums[i];
        int v2 = nums[left];
        int v3 = nums[right];

        while (left < right) {
            int res = v1 + v2 + v3;
            int sub = target - res;
            int diff = abs(sub);

            if (diff < minDiffValue) {
                if (diff == 0)
                    return res;

                sum = res;
                minDiffValue = diff;
            }

            if (sub > 0)
                v2 = nums[++left];
            else
                v3 = nums[--right];
        }
    }

    return sum;
}

/* Given an array of integers "nums" and an integer "target", return indices of the two
 * numbers such that they add up to "target". You may assume that each input would have
 * exactly one solution, and you may not use the same element twice. You can return the
 * answer in any order.
 */
/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int *twoSum2(int *nums, int numsSize, int target, int *returnSize)
{
    int *res = (int *)malloc(sizeof(int) * 2);
    int *idxs = (int *)malloc(sizeof(int) * numsSize);
    for (int i = 0; i < numsSize; i++)
        idxs[i] = i;

    heap_sort_int(nums, idxs, numsSize);

    for (int i = 0, key; i < numsSize - 1; i++) {
        key = target - nums[i];
        int *p = binary_search_int(nums + (i + 1), key, numsSize - (i + 1));
        if (p != NULL) {
            res[0] = idxs[i];
            res[1] = idxs[p - nums];
            *returnSize = 2;
            free(idxs);
            return res;
        }
    }

    return NULL;
}

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* twoSum3(int* nums, int numsSize, int target, int* returnSize){
    *returnSize = 2;
    int* res = (int *)malloc(2 * sizeof(int));
    for (int i = 0; i < numsSize - 1; i++) {
        for (int j = i + 1; j < numsSize; j++) {
            if ((nums[i] + nums[j]) == target) {
                res[0] = i;
                res[1] = j;
                return res;
            } else if ((nums[numsSize - 1 - i] + nums[numsSize - 1 - j]) == target) {
                res[0] = numsSize - 1 - j;
                res[1] = numsSize - 1 - i;
                return res;
            }
        }
    }
    return res;
}

int addDigits(int num)
{
    int sum;
    do {
        sum = 0;
        while (num > 0) {
            sum += (num % 10);
            num = num / 10;
        }
        num = sum;
    } while (sum > 10);

    return sum;
}

int addDigits2(int num)
{
    return (num - 1) % 9 + 1;
}

inline int sumDigits(int val)
{
    int sum = 0;
    while (val) {
        sum += val % 10;
        val /= 10;
    }
    return sum;
}

int getLucky(char * s, int k)
{
    int sum = 0;
    for (int j = 0; j < strlen(s); j++) {
        int val = s[j] - 'a' + 1;
        sum += val < 10 ? val : sumDigits(val);
    }

    while (k >= 2) {
        sum = sumDigits(sum);
        k--;
    }
    return sum;
}

void c_test_getLucky(void)
{
    printf("%d\n", getLucky("hvmhoasabaymnmsd", 1));
    printf("%d\n", getLucky2("hvmhoasabaymnmsd", 1));
}

// int nums[] = {4,0,5,-5,3,3,0,-4,-5};
int nums[] = {-1000,-5,-5,-5,-5,-5,-5,-1,-1,-1};

void c_test_threeSumClosest(void)
{
    int res = threeSumClosest(nums, sizeof(nums) / sizeof(int), -14);
    printf("res = %d\n", res);
}

void c_test_generate_dynamic_random_array_int(void)
{
    int *nums, numsSize;
    generate_dynamic_random_array_int(&nums, &numsSize, 100);

    for (int i = 0; i < numsSize; i++) {
        if (i && (i % 10) == 0)
            printf("\n");
        printf("%5d ", nums[i]);
    }
}

void c_test_binary_search(void)
{
    int *nums, numsSize = 10;
    generate_fix_random_array_int(&nums, numsSize);
    for (int i = 0; i < numsSize; i++) {
        if (i && (i % 10) == 0)
            printf("\n");
        printf("%5d ", nums[i]);
    }
    printf("\n");

    int *idxs = (int *)malloc(sizeof(int) * numsSize);
    for (int i = 0; i < numsSize; i++)
        idxs[i] = i;

    heap_sort_int(nums, idxs, numsSize);
    for (int i = 0; i < numsSize; i++) {
        if (i && (i % 10) == 0)
            printf("\n");
        printf("%5d ", nums[i]);
    }
    printf("\n");

    for (int i = 0; i < numsSize; i++) {
        int *p = binary_search_int(nums, nums[i], numsSize);
        if (p != NULL)
            printf("%d %d\n", p - nums, nums[p - nums]);
    }
    printf("\n");
}

void c_test_heap_sort_int(void)
{
    int *nums, *idxs, numsSize;
    generate_dynamic_random_array_int(&nums, &numsSize, 999);
    for (int i = 0; i < numsSize; i++) {
        if (i && (i % 10) == 0)
            printf("\n");
        printf("%5d ", nums[i]);
    }
    printf("\n\n");
    idxs = (int *)malloc(sizeof(int) * numsSize);
    for (int i = 0; i < numsSize; i++)
        idxs[i] = i;

    heap_sort_int(nums, idxs, numsSize);
    for (int i = 0; i < numsSize; i++) {
        if (i && (i % 10) == 0)
            printf("\n");
        printf("%5d ", nums[i]);
    }
    printf("\n\n");
}

void c_test_twoSum2(void)
{
    int *nums, *res, numsSize, target, returnSize;
    srand(time(0));

    numsSize = 10;
    generate_fix_random_array_int(&nums, numsSize);
    // generate_dynamic_random_array_int(&nums, &numsSize, 10);
    int i1, i2;
    i1 = rand() % numsSize;
    while (1) {
        i2 = rand() % numsSize;
        if (i1 != i2)
            break;
    }

    target = nums[i1] + nums[i2];
    for (int i = 0; i < numsSize; i++) {
        if (i && (i % 10) == 0)
            printf("\n");
        printf("%5d ", nums[i]);
    }
    printf("\n");
    printf("tar=%d i=%d %d v=%d %d\n",
        target, i1, i2, nums[i1], nums[i2]);

    res = twoSum2(nums, numsSize, target, &returnSize);
    for (int i = 0; i < numsSize; i++) {
        if (i && (i % 10) == 0)
            printf("\n");
        printf("%5d ", nums[i]);
    }
    printf("\n");
    printf("tar=%d r=%d %d\n",
        target, res[0], res[1]);

    free(nums);
    free(res);
}

void c_test(void)
{
    printf("%s start >>>\n", __FUNCTION__);
    printf("%s end <<<\n", __FUNCTION__);
}

// Calculate sum of sequence 1*2 + 2*3 + 3*4 +...+(n-1)*n
int sum_of_sequence_1(int n)
{
    int i, r = 0;

    for (i = 1; i < n; i++)
        r = r + i * (i + 1);

    return r;
}

int sum_of_sequence_2(int n)
{
    if (n <= 1)
        return 0;
    else
        return (n - 1) * n + sum_of_sequence_2(n - 1);
}

void c_test_quanta(int n)
{
    printf("sum1: %d\n", sum_of_sequence_1(n));
    printf("sum2: %d\n", sum_of_sequence_2(n));
}

void *InitBuffer()
{
    // char _buf[10] = {};
    static char _buf[13] = {};
    return _buf;
}

// If this is a main function
int c_test_inventec()
{
    // char *buf = InitBuffer();
    char *buf = (char *)InitBuffer();
    strcpy(buf, "Hello World\n");
    printf("%s", buf);
}

void c_test_elen_1(void)
{
    int a[] = {6, 7, 8, 9, 10};
    int *p = a;
    printf("%X\n",a);
    printf("%X\n",&a);
    *(p++)+=123;
    // *(++p)+=123;

    printf("%d\n", *p);

    for (int i = 0; i < 5; i++)
        printf("%d ", a[i]);


    printf("\n");
    printf("%d\n", *p);
}

#define X_MAX   (10)
#define Y_MAX   (9999)
#define VAL     (20)

void c_test_elen_2(void)
{
    int v, x, y;

    // Ans 1
    v = VAL;
    y = v / X_MAX;
    x = v % X_MAX;
    printf("x=%d, y=%d\n", x, y);

    // Ans 2
    for (y = 0; y < Y_MAX; y++) {
        for (x = 0; x < X_MAX; x++) {
            if (v == y * X_MAX + x)
                goto exit;
        }
    }

exit:
    printf("x=%d, y=%d\n", x, y);

    // Ans3
}