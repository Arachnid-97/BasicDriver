#ifndef __FILTER_H
#define __FILTER_H


#define Data_Typedef		float


Data_Typedef Get_AD(void);
Data_Typedef Limit_Filter( double Value, int Range );
Data_Typedef Median_Filter( int Count );
Data_Typedef Mean_Filter( int Count );
Data_Typedef Recursive_Mean_Filter( int Count );
Data_Typedef Median_Mean_Filter( int Count );
Data_Typedef Limiting_Mean_Filter( int Range, int Count );
Data_Typedef Low_Pass_Filter( Data_Typedef Value, float Factor );
Data_Typedef Weighted_Recursive_Mean_Filter( char *Weight_factor, int Count );
Data_Typedef Debounce_Filter( Data_Typedef Value, int Count );
Data_Typedef Limiter_Debounce_Filter( Data_Typedef Value, int Range, int Count );


#endif /* __FILTER_H */

