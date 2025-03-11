namespace Extensions
{
    public static class ExtensionMethods
    {
        public static bool Contains(this CartoType.Rect aRect, double aX, double aY)
        {
            return aX > aRect.MinX && aY > aRect.MinY && aX <= aRect.MaxX && aY <= aRect.MaxY;
        }
    }
}
