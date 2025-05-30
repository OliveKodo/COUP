<div dir="rtl">
#tomergal40@gmail.com
# משחק Coup 

## תיאור כללי
Coup הוא משחק אסטרטגיה פוליטית שבו השחקנים מתחרים על שליטה במדינה באמצעות מניפולציות, בלופים ופעולות מיוחדות. כל שחקן מקבל תפקיד עם יכולות ייחודיות, ומטרתו להישאר השחקן האחרון במשחק.

## תכונות המשחק

### תפקידים ויכולות

#### Governor
- Tax מיוחד: מקבל 3 מטבעות במקום 2.
- Undo: יכול לבטל פעולות tax של שחקנים אחרים.
- Arrest: יכול לעצור שחקנים.

#### Spy
- SpyOn: רואה כמה מטבעות יש לשחקן אחר.
- Arrest: עוצר שחקן וגורם לו לאבד מטבע.
- Tax רגיל: מקבל 2 מטבעות.

#### Baron
- Invest: משקיע 3 מטבעות ומקבל 6.
- Sanction: מטיל סנקציות.
- Compensation: מקבל מטבע כשמוטלת עליו סנקציה.

#### General
- Immunity: חסין מ־Coup במצבים מסוימים.
- Arrest Recovery: מקבל מטבעות אחרי מעצר.
- פעולות צבאיות.

#### Judge
- Undo Bribe: מבטל פעולות שוחד.
- Sanction: מטיל סנקציות.
- Legal Authority: סמכות לשבור פעולות לא חוקיות.

#### Merchant
- Bonus Income: מקבל בונוס אם יש לו 3+ מטבעות.
- Double Arrest Fee: משלם כפול בעת מעצר.
- יכולות מסחר.

### מכניקות משחק

- Gather – איסוף מטבע (לכולם).
- Tax – איסוף 2 מטבעות (Governor מקבל 3).
- Bribe – תשלום 4 מטבעות להפעלת יתרון.
- Coup – תשלום 7 מטבעות להדחת שחקן.
- Sanction – שלילת פעולות משחקן.
- Arrest – עצירת שחקן וגרימת קנס.
- Undo – ביטול פעולה של שחקן אחר.
- 10+ Coins Rule – חובה לבצע Coup כשיש 10 מטבעות או יותר.
- Elimination – שחקן שהודח יוצא מהמשחק.
- ניהול תורות – מערכת מחזורית כולל דילוג על שחקנים מודחים.

## מבנה הפרויקט
include/ - קבצי header
src/ -  המחלקות
test/ - test , main
gui/ - ממשק גרפי 
obj/ - קבצי object מהקומפילציה



### פקודות Make
make Main       - הדגמה מלאה של המשחק
make test       - הרצת בדיקות יחידה
make valgrind   - בדיקת זיכרון
make CoupGUI    - הרצת ממשק גרפי
make clean      - ניקוי קבצים
make all        - בנייה מלאה


</div>
