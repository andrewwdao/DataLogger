#include <bits/stdc++.h>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Must specify amount of run\n");
        return 1;
    }

    ios_base::sync_with_stdio(false);
    cin.tie(0);

    freopen("record", "r", stdin);
    freopen("record_", "w", stdout);

    vector<int> v;

    for (int i = 0; i < atoi(argv[1])*100; i++)
    {
        int a; cin >> a;
        if (a == 'q') continue;
        v.push_back(a);
    }

    sort(v.begin(), v.end());
    for (int i = 1; i < atoi(argv[1])*100; i++)
    {
        cout << i << "\n";
        // if (v[i] - v[i-1] - 1)
        // {
        //     cout << "WRONG! " << v[i-1] << " AND " << v[i] << "\n";
        // }
    }

    printf("DONE\n");

    return 0;
}