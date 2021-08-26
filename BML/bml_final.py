# -*- coding: utf-8 -*-
"""BML_final.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/10qNnyhNpmFIan8IrwA7-PXbfak6mksH3
"""

# Commented out IPython magic to ensure Python compatibility.
# %tensorflow_version 1.x
from google.colab import drive
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
from sklearn.linear_model import Lasso
from sklearn.preprocessing import StandardScaler, MinMaxScaler
import seaborn as sns
import statsmodels.api as sm
from sklearn.model_selection import GridSearchCV, RepeatedKFold, KFold, cross_val_score

from google.colab import drive

data = pd.read_csv('/content/For_FilnalReport.csv',index_col=0)
test_data = pd.read_csv('/content/test_data_x.csv',index_col=0)

data = data.dropna(axis=0)
features = list(data.columns)[:912]

x = data.loc[:,features]
y = data['ERLOTINIB']

x

y

data_x = data.loc[:,['RBM5','CDC27','CREBBP','MED24','CSDE1']]

scaler = StandardScaler()
scaled_val = scaler.fit_transform(data_x)
scaled_x = pd.DataFrame(scaled_val, columns=data_x.columns)
sns.boxplot(data=scaled_x)
plt.title('Distribution of some genes expression')
plt.show()

mut = list(data.columns)[452:455]
mut = data.loc[:,mut]
sns.pairplot(mut)
plt.figure(figsize = (1,1))
plt.show()

data_y = data.loc[:,['ERLOTINIB']]

scaler = StandardScaler()
scaled_val = scaler.fit_transform(data_y)
scaled_y = pd.DataFrame(scaled_val, columns=data_y.columns)
sns.boxplot(data=scaled_y)
plt.title('Distribution of drug response')
plt.show()

def train_model(model, param_grid=[], X=[], y=[], 
                splits=10, repeats=1):

    # create cross-validation method
    rkfold = RepeatedKFold(n_splits=splits, n_repeats=repeats, random_state=123)
    
    # perform a grid search if param_grid given
    if len(param_grid)>0:
        # setup grid search parameters
        gsearch = GridSearchCV(model, param_grid, cv=rkfold,
                               scoring='r2',
                               verbose=True, return_train_score=True, n_jobs=4)

        gsearch.fit(X,y)
        model = gsearch.best_estimator_        
        best_idx = gsearch.best_index_
        # get cv-scores for best model
        grid_results = pd.DataFrame(gsearch.cv_results_)       
        cv_mean = abs(grid_results.loc[best_idx,'mean_test_score'])
        cv_std = grid_results.loc[best_idx,'std_test_score']

    # no grid search, just cross-val score for given model    
    else:
        grid_results = []
        cv_results = cross_val_score(model, X, y, scoring='r2', cv=rkfold)
        cv_mean = abs(np.mean(cv_results))
        cv_std = np.std(cv_results)
    
    # combine mean and std cv-score in to a pandas series
    cv_score = pd.Series({'mean':cv_mean,'std':cv_std})

    # predict y using the fitted model
    y_pred = model.predict(X)
    
    # print stats on model performance         
    print('----------------------')
    print(model)
    print('----------------------')
    
    # residual plots
    y_pred = pd.Series(y_pred,index=y.index)
    resid = y - y_pred
    mean_resid = resid.mean()
    std_resid = resid.std()
    
    plt.figure(figsize=(12,4))
    ax_121 = plt.subplot(1,2,1)
    plt.plot(y_pred,y,'.')
    plt.xlabel('y')
    plt.ylabel('y_pred');
    plt.title('corr = {:.3f}'.format(np.corrcoef(y,y_pred)[0][1]))
    
    #cv_results = cross_val_predict(model, X, y, n_jobs = 4, cv=splits)
    
    ax_122=plt.subplot(1,2,2)
    plt.plot(y_pred,y-y_pred,'.')
    plt.ylim((-4,4))
    plt.xlabel('y_pred')
    plt.ylabel('y - y_pred');
    plt.title('std resid = {:.3f}'.format(std_resid))
    plt.show()

    return model, cv_score, grid_results
######
splits = 5
repeats = 5
model=Lasso()
lambda_range = np.arange(0,1,0.001)
param_grid = {
    'alpha': lambda_range
}
best_model,cv_score,grid_results = train_model(model, param_grid=param_grid, splits=splits, repeats=repeats, X=x, y=y)

plt.plot(lambda_range, abs(grid_results['mean_test_score']))
plt.xlabel('lambda')
plt.ylabel('cv-score')
plt.show()
print()
print('Best Score')
print(cv_score)
print()
print('Lambda')
print(best_model.alpha)

x_train, x_test, y_train, y_test = train_test_split(x, y, test_size = 0.2, random_state = 0)

model = LinearRegression()
model.fit(x_train, y_train)

feature_names = list(x)
print(feature_names)
feature_coef = model.coef_
importance = pd.Series(feature_coef, index = feature_names)
df_importance = importance.sort_values(axis=0)
df_importance[:10]

predict_model = model.predict(x_test)

print("y절편 : ",model.intercept_)

print(test_data['ERLOTINIB'])
features = list(test_data.columns)[:912]
td = test_data.loc[:,features]
predict_ex = model.predict(td)
print(predict_ex)

plt.scatter(y_test, predict_model, alpha=0.4)
plt.xlabel("Actual ERLOTINIB")
plt.ylabel("Predicted ERLOTINIB")
plt.show()

lasso = Lasso(alpha=0.3, max_iter=10000000).fit(x_train, y_train)
print("train R^2: {:.2f}".format(lasso.score(x_train, y_train)))
print("test R^2: {:.2f}".format(lasso.score(x_test, y_test)))
print("number of used characteristic : {}".format(np.sum(lasso.coef_ != 0)))



y_hat = model.predict(x_test)
plt.figure(figsize = (10,5))
plt.plot(y_test, label = 'y_test')
plt.plot(y_hat, label = 'y_hat')
plt.legend(loc = 'upper right')
plt.show()