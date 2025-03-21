/* header file for clapack 3.2.1 */

#ifndef __CLAPACK_H
#define __CLAPACK_H

int caxpy_(integer *n, complex *ca, complex *cx, integer *incx, complex *cy, integer *incy);

int ccopy_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy);

VOID cdotc_(complex *ret_val, integer *n, complex *cx, integer *incx, complex *cy, integer *incy);

VOID cdotu_(complex *ret_val, integer *n, complex *cx, integer *incx, complex *cy, integer *incy);

int cgbmv_(char *trans, integer *m, integer *n, integer *kl, integer *ku, complex *alpha, complex *a, integer *lda,
    complex *x, integer *incx, complex *beta, complex *y, integer *incy);

int cgemm_(char *transa, char *transb, integer *m, integer *n, integer *k, complex *alpha, complex *a, integer *lda,
    complex *b, integer *ldb, complex *beta, complex *c__, integer *ldc);

int cgemv_(char *trans, integer *m, integer *n, complex *alpha, complex *a, integer *lda, complex *x, integer *incx,
    complex *beta, complex *y, integer *incy);

int cgerc_(integer *m, integer *n, complex *alpha, complex *x, integer *incx, complex *y, integer *incy, complex *a,
    integer *lda);

int cgeru_(integer *m, integer *n, complex *alpha, complex *x, integer *incx, complex *y, integer *incy, complex *a,
    integer *lda);

int chbmv_(char *uplo, integer *n, integer *k, complex *alpha, complex *a, integer *lda, complex *x, integer *incx,
    complex *beta, complex *y, integer *incy);

int chemm_(char *side, char *uplo, integer *m, integer *n, complex *alpha, complex *a, integer *lda, complex *b,
    integer *ldb, complex *beta, complex *c__, integer *ldc);

int chemv_(char *uplo, integer *n, complex *alpha, complex *a, integer *lda, complex *x, integer *incx, complex *beta,
    complex *y, integer *incy);

int cher_(char *uplo, integer *n, real *alpha, complex *x, integer *incx, complex *a, integer *lda);

int cher2_(char *uplo, integer *n, complex *alpha, complex *x, integer *incx, complex *y, integer *incy, complex *a,
    integer *lda);

int cher2k_(char *uplo, char *trans, integer *n, integer *k, complex *alpha, complex *a, integer *lda, complex *b,
    integer *ldb, real *beta, complex *c__, integer *ldc);

int cherk_(char *uplo, char *trans, integer *n, integer *k, real *alpha, complex *a, integer *lda, real *beta,
    complex *c__, integer *ldc);

int chpmv_(char *uplo, integer *n, complex *alpha, complex *ap, complex *x, integer *incx, complex *beta, complex *y,
    integer *incy);

int chpr_(char *uplo, integer *n, real *alpha, complex *x, integer *incx, complex *ap);

int chpr2_(char *uplo, integer *n, complex *alpha, complex *x, integer *incx, complex *y, integer *incy, complex *ap);

int crotg_(complex *ca, complex *cb, real *c__, complex *s);

int cscal_(integer *n, complex *ca, complex *cx, integer *incx);

int csrot_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy, real *c__, real *s);

int csscal_(integer *n, real *sa, complex *cx, integer *incx);

int cswap_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy);

int csymm_(char *side, char *uplo, integer *m, integer *n, complex *alpha, complex *a, integer *lda, complex *b,
    integer *ldb, complex *beta, complex *c__, integer *ldc);

int csyr2k_(char *uplo, char *trans, integer *n, integer *k, complex *alpha, complex *a, integer *lda, complex *b,
    integer *ldb, complex *beta, complex *c__, integer *ldc);

int csyrk_(char *uplo, char *trans, integer *n, integer *k, complex *alpha, complex *a, integer *lda, complex *beta,
    complex *c__, integer *ldc);

int ctbmv_(
    char *uplo, char *trans, char *diag, integer *n, integer *k, complex *a, integer *lda, complex *x, integer *incx);

int ctbsv_(
    char *uplo, char *trans, char *diag, integer *n, integer *k, complex *a, integer *lda, complex *x, integer *incx);

int ctpmv_(char *uplo, char *trans, char *diag, integer *n, complex *ap, complex *x, integer *incx);

int ctpsv_(char *uplo, char *trans, char *diag, integer *n, complex *ap, complex *x, integer *incx);

int ctrmm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, complex *alpha, complex *a,
    integer *lda, complex *b, integer *ldb);

int ctrmv_(char *uplo, char *trans, char *diag, integer *n, complex *a, integer *lda, complex *x, integer *incx);

int ctrsm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, complex *alpha, complex *a,
    integer *lda, complex *b, integer *ldb);

int ctrsv_(char *uplo, char *trans, char *diag, integer *n, complex *a, integer *lda, complex *x, integer *incx);

doublereal dasum_(integer *n, doublereal *dx, integer *incx);

int daxpy_(integer *n, doublereal *da, doublereal *dx, integer *incx, doublereal *dy, integer *incy);

doublereal dcabs1_(doublecomplex *z__);

int dcopy_(integer *n, doublereal *dx, integer *incx, doublereal *dy, integer *incy);

doublereal ddot_(integer *n, doublereal *dx, integer *incx, doublereal *dy, integer *incy);

int dgbmv_(char *trans, integer *m, integer *n, integer *kl, integer *ku, doublereal *alpha, doublereal *a,
    integer *lda, doublereal *x, integer *incx, doublereal *beta, doublereal *y, integer *incy);

int dgemm_(char *transa, char *transb, integer *m, integer *n, integer *k, doublereal *alpha, doublereal *a,
    integer *lda, doublereal *b, integer *ldb, doublereal *beta, doublereal *c__, integer *ldc);

int dgemv_(char *trans, integer *m, integer *n, doublereal *alpha, doublereal *a, integer *lda, doublereal *x,
    integer *incx, doublereal *beta, doublereal *y, integer *incy);

int dger_(integer *m, integer *n, doublereal *alpha, doublereal *x, integer *incx, doublereal *y, integer *incy,
    doublereal *a, integer *lda);

doublereal dnrm2_(integer *n, doublereal *x, integer *incx);

int drot_(integer *n, doublereal *dx, integer *incx, doublereal *dy, integer *incy, doublereal *c__, doublereal *s);

int drotg_(doublereal *da, doublereal *db, doublereal *c__, doublereal *s);

int drotm_(integer *n, doublereal *dx, integer *incx, doublereal *dy, integer *incy, doublereal *dparam);

int drotmg_(doublereal *dd1, doublereal *dd2, doublereal *dx1, doublereal *dy1, doublereal *dparam);

int dsbmv_(char *uplo, integer *n, integer *k, doublereal *alpha, doublereal *a, integer *lda, doublereal *x,
    integer *incx, doublereal *beta, doublereal *y, integer *incy);

int dscal_(integer *n, doublereal *da, doublereal *dx, integer *incx);

doublereal dsdot_(integer *n, real *sx, integer *incx, real *sy, integer *incy);

int dspmv_(char *uplo, integer *n, doublereal *alpha, doublereal *ap, doublereal *x, integer *incx, doublereal *beta,
    doublereal *y, integer *incy);

int dspr_(char *uplo, integer *n, doublereal *alpha, doublereal *x, integer *incx, doublereal *ap);

int dspr2_(char *uplo, integer *n, doublereal *alpha, doublereal *x, integer *incx, doublereal *y, integer *incy,
    doublereal *ap);

int dswap_(integer *n, doublereal *dx, integer *incx, doublereal *dy, integer *incy);

int dsymm_(char *side, char *uplo, integer *m, integer *n, doublereal *alpha, doublereal *a, integer *lda,
    doublereal *b, integer *ldb, doublereal *beta, doublereal *c__, integer *ldc);

int dsymv_(char *uplo, integer *n, doublereal *alpha, doublereal *a, integer *lda, doublereal *x, integer *incx,
    doublereal *beta, doublereal *y, integer *incy);

int dsyr_(char *uplo, integer *n, doublereal *alpha, doublereal *x, integer *incx, doublereal *a, integer *lda);

int dsyr2_(char *uplo, integer *n, doublereal *alpha, doublereal *x, integer *incx, doublereal *y, integer *incy,
    doublereal *a, integer *lda);

int dsyr2k_(char *uplo, char *trans, integer *n, integer *k, doublereal *alpha, doublereal *a, integer *lda,
    doublereal *b, integer *ldb, doublereal *beta, doublereal *c__, integer *ldc);

int dsyrk_(char *uplo, char *trans, integer *n, integer *k, doublereal *alpha, doublereal *a, integer *lda,
    doublereal *beta, doublereal *c__, integer *ldc);

int dtbmv_(char *uplo, char *trans, char *diag, integer *n, integer *k, doublereal *a, integer *lda, doublereal *x,
    integer *incx);

int dtbsv_(char *uplo, char *trans, char *diag, integer *n, integer *k, doublereal *a, integer *lda, doublereal *x,
    integer *incx);

int dtpmv_(char *uplo, char *trans, char *diag, integer *n, doublereal *ap, doublereal *x, integer *incx);

int dtpsv_(char *uplo, char *trans, char *diag, integer *n, doublereal *ap, doublereal *x, integer *incx);

int dtrmm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, doublereal *alpha, doublereal *a,
    integer *lda, doublereal *b, integer *ldb);

int dtrmv_(char *uplo, char *trans, char *diag, integer *n, doublereal *a, integer *lda, doublereal *x, integer *incx);

int dtrsm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, doublereal *alpha, doublereal *a,
    integer *lda, doublereal *b, integer *ldb);

int dtrsv_(char *uplo, char *trans, char *diag, integer *n, doublereal *a, integer *lda, doublereal *x, integer *incx);

doublereal dzasum_(integer *n, doublecomplex *zx, integer *incx);

doublereal dznrm2_(integer *n, doublecomplex *x, integer *incx);

integer icamax_(integer *n, complex *cx, integer *incx);

integer idamax_(integer *n, doublereal *dx, integer *incx);

integer isamax_(integer *n, real *sx, integer *incx);

integer izamax_(integer *n, doublecomplex *zx, integer *incx);

logical lsame_(char *ca, char *cb);

doublereal sasum_(integer *n, real *sx, integer *incx);

int saxpy_(integer *n, real *sa, real *sx, integer *incx, real *sy, integer *incy);

doublereal scabs1_(complex *z__);

doublereal scasum_(integer *n, complex *cx, integer *incx);

doublereal scnrm2_(integer *n, complex *x, integer *incx);

int scopy_(integer *n, real *sx, integer *incx, real *sy, integer *incy);

doublereal sdot_(integer *n, real *sx, integer *incx, real *sy, integer *incy);

doublereal sdsdot_(integer *n, real *sb, real *sx, integer *incx, real *sy, integer *incy);

int sgbmv_(char *trans, integer *m, integer *n, integer *kl, integer *ku, real *alpha, real *a, integer *lda, real *x,
    integer *incx, real *beta, real *y, integer *incy);

int sgemm_(char *transa, char *transb, integer *m, integer *n, integer *k, real *alpha, real *a, integer *lda, real *b,
    integer *ldb, real *beta, real *c__, integer *ldc);

int sgemv_(char *trans, integer *m, integer *n, real *alpha, real *a, integer *lda, real *x, integer *incx, real *beta,
    real *y, integer *incy);

int sger_(integer *m, integer *n, real *alpha, real *x, integer *incx, real *y, integer *incy, real *a, integer *lda);

doublereal snrm2_(integer *n, real *x, integer *incx);

int srot_(integer *n, real *sx, integer *incx, real *sy, integer *incy, real *c__, real *s);

int srotg_(real *sa, real *sb, real *c__, real *s);

int srotm_(integer *n, real *sx, integer *incx, real *sy, integer *incy, real *sparam);

int srotmg_(real *sd1, real *sd2, real *sx1, real *sy1, real *sparam);

int ssbmv_(char *uplo, integer *n, integer *k, real *alpha, real *a, integer *lda, real *x, integer *incx, real *beta,
    real *y, integer *incy);

int sscal_(integer *n, real *sa, real *sx, integer *incx);

int sspmv_(char *uplo, integer *n, real *alpha, real *ap, real *x, integer *incx, real *beta, real *y, integer *incy);

int sspr_(char *uplo, integer *n, real *alpha, real *x, integer *incx, real *ap);

int sspr2_(char *uplo, integer *n, real *alpha, real *x, integer *incx, real *y, integer *incy, real *ap);

int sswap_(integer *n, real *sx, integer *incx, real *sy, integer *incy);

int ssymm_(char *side, char *uplo, integer *m, integer *n, real *alpha, real *a, integer *lda, real *b, integer *ldb,
    real *beta, real *c__, integer *ldc);

int ssymv_(char *uplo, integer *n, real *alpha, real *a, integer *lda, real *x, integer *incx, real *beta, real *y,
    integer *incy);

int ssyr_(char *uplo, integer *n, real *alpha, real *x, integer *incx, real *a, integer *lda);

int ssyr2_(char *uplo, integer *n, real *alpha, real *x, integer *incx, real *y, integer *incy, real *a, integer *lda);

int ssyr2k_(char *uplo, char *trans, integer *n, integer *k, real *alpha, real *a, integer *lda, real *b, integer *ldb,
    real *beta, real *c__, integer *ldc);

int ssyrk_(char *uplo, char *trans, integer *n, integer *k, real *alpha, real *a, integer *lda, real *beta, real *c__,
    integer *ldc);

int stbmv_(char *uplo, char *trans, char *diag, integer *n, integer *k, real *a, integer *lda, real *x, integer *incx);

int stbsv_(char *uplo, char *trans, char *diag, integer *n, integer *k, real *a, integer *lda, real *x, integer *incx);

int stpmv_(char *uplo, char *trans, char *diag, integer *n, real *ap, real *x, integer *incx);

int stpsv_(char *uplo, char *trans, char *diag, integer *n, real *ap, real *x, integer *incx);

int strmm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, real *alpha, real *a, integer *lda,
    real *b, integer *ldb);

int strmv_(char *uplo, char *trans, char *diag, integer *n, real *a, integer *lda, real *x, integer *incx);

int strsm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, real *alpha, real *a, integer *lda,
    real *b, integer *ldb);

int strsv_(char *uplo, char *trans, char *diag, integer *n, real *a, integer *lda, real *x, integer *incx);

int xerbla_(char *srname, integer *info);

int xerbla_array__(char *srname_array__, integer *srname_len__, integer *info, ftnlen srname_array_len);

int zaxpy_(integer *n, doublecomplex *za, doublecomplex *zx, integer *incx, doublecomplex *zy, integer *incy);

int zcopy_(integer *n, doublecomplex *zx, integer *incx, doublecomplex *zy, integer *incy);

VOID zdotc_(doublecomplex *ret_val, integer *n, doublecomplex *zx, integer *incx, doublecomplex *zy, integer *incy);

VOID zdotu_(doublecomplex *ret_val, integer *n, doublecomplex *zx, integer *incx, doublecomplex *zy, integer *incy);

int zdrot_(
    integer *n, doublecomplex *cx, integer *incx, doublecomplex *cy, integer *incy, doublereal *c__, doublereal *s);

int zdscal_(integer *n, doublereal *da, doublecomplex *zx, integer *incx);

int zgbmv_(char *trans, integer *m, integer *n, integer *kl, integer *ku, doublecomplex *alpha, doublecomplex *a,
    integer *lda, doublecomplex *x, integer *incx, doublecomplex *beta, doublecomplex *y, integer *incy);

int zgemm_(char *transa, char *transb, integer *m, integer *n, integer *k, doublecomplex *alpha, doublecomplex *a,
    integer *lda, doublecomplex *b, integer *ldb, doublecomplex *beta, doublecomplex *c__, integer *ldc);

int zgemv_(char *trans, integer *m, integer *n, doublecomplex *alpha, doublecomplex *a, integer *lda, doublecomplex *x,
    integer *incx, doublecomplex *beta, doublecomplex *y, integer *incy);

int zgerc_(integer *m, integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *y,
    integer *incy, doublecomplex *a, integer *lda);

int zgeru_(integer *m, integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *y,
    integer *incy, doublecomplex *a, integer *lda);

int zhbmv_(char *uplo, integer *n, integer *k, doublecomplex *alpha, doublecomplex *a, integer *lda, doublecomplex *x,
    integer *incx, doublecomplex *beta, doublecomplex *y, integer *incy);

int zhemm_(char *side, char *uplo, integer *m, integer *n, doublecomplex *alpha, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *beta, doublecomplex *c__, integer *ldc);

int zhemv_(char *uplo, integer *n, doublecomplex *alpha, doublecomplex *a, integer *lda, doublecomplex *x,
    integer *incx, doublecomplex *beta, doublecomplex *y, integer *incy);

int zher_(char *uplo, integer *n, doublereal *alpha, doublecomplex *x, integer *incx, doublecomplex *a, integer *lda);

int zher2_(char *uplo, integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *y,
    integer *incy, doublecomplex *a, integer *lda);

int zher2k_(char *uplo, char *trans, integer *n, integer *k, doublecomplex *alpha, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublereal *beta, doublecomplex *c__, integer *ldc);

int zherk_(char *uplo, char *trans, integer *n, integer *k, doublereal *alpha, doublecomplex *a, integer *lda,
    doublereal *beta, doublecomplex *c__, integer *ldc);

int zhpmv_(char *uplo, integer *n, doublecomplex *alpha, doublecomplex *ap, doublecomplex *x, integer *incx,
    doublecomplex *beta, doublecomplex *y, integer *incy);

int zhpr_(char *uplo, integer *n, doublereal *alpha, doublecomplex *x, integer *incx, doublecomplex *ap);

int zhpr2_(char *uplo, integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *y,
    integer *incy, doublecomplex *ap);

int zrotg_(doublecomplex *ca, doublecomplex *cb, doublereal *c__, doublecomplex *s);

int zscal_(integer *n, doublecomplex *za, doublecomplex *zx, integer *incx);

int zswap_(integer *n, doublecomplex *zx, integer *incx, doublecomplex *zy, integer *incy);

int zsymm_(char *side, char *uplo, integer *m, integer *n, doublecomplex *alpha, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *beta, doublecomplex *c__, integer *ldc);

int zsyr2k_(char *uplo, char *trans, integer *n, integer *k, doublecomplex *alpha, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *beta, doublecomplex *c__, integer *ldc);

int zsyrk_(char *uplo, char *trans, integer *n, integer *k, doublecomplex *alpha, doublecomplex *a, integer *lda,
    doublecomplex *beta, doublecomplex *c__, integer *ldc);

int ztbmv_(char *uplo, char *trans, char *diag, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *x, integer *incx);

int ztbsv_(char *uplo, char *trans, char *diag, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *x, integer *incx);

int ztpmv_(char *uplo, char *trans, char *diag, integer *n, doublecomplex *ap, doublecomplex *x, integer *incx);

int ztpsv_(char *uplo, char *trans, char *diag, integer *n, doublecomplex *ap, doublecomplex *x, integer *incx);

int ztrmm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, doublecomplex *alpha,
    doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb);

int ztrmv_(
    char *uplo, char *trans, char *diag, integer *n, doublecomplex *a, integer *lda, doublecomplex *x, integer *incx);

int ztrsm_(char *side, char *uplo, char *transa, char *diag, integer *m, integer *n, doublecomplex *alpha,
    doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb);

int ztrsv_(
    char *uplo, char *trans, char *diag, integer *n, doublecomplex *a, integer *lda, doublecomplex *x, integer *incx);

int cbdsqr_(char *uplo, integer *n, integer *ncvt, integer *nru, integer *ncc, real *d__, real *e, complex *vt,
    integer *ldvt, complex *u, integer *ldu, complex *c__, integer *ldc, real *rwork, integer *info);

int cgbbrd_(char *vect, integer *m, integer *n, integer *ncc, integer *kl, integer *ku, complex *ab, integer *ldab,
    real *d__, real *e, complex *q, integer *ldq, complex *pt, integer *ldpt, complex *c__, integer *ldc, complex *work,
    real *rwork, integer *info);

int cgbcon_(char *norm, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, integer *ipiv, real *anorm,
    real *rcond, complex *work, real *rwork, integer *info);

int cgbequ_(integer *m, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, real *r__, real *c__,
    real *rowcnd, real *colcnd, real *amax, integer *info);

int cgbequb_(integer *m, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, real *r__, real *c__,
    real *rowcnd, real *colcnd, real *amax, integer *info);

int cgbrfs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, complex *ab, integer *ldab, complex *afb,
    integer *ldafb, integer *ipiv, complex *b, integer *ldb, complex *x, integer *ldx, real *ferr, real *berr,
    complex *work, real *rwork, integer *info);

int cgbrfsx_(char *trans, char *equed, integer *n, integer *kl, integer *ku, integer *nrhs, complex *ab, integer *ldab,
    complex *afb, integer *ldafb, integer *ipiv, real *r__, real *c__, complex *b, integer *ldb, complex *x,
    integer *ldx, real *rcond, real *berr, integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__,
    integer *nparams, real *params, complex *work, real *rwork, integer *info);

int cgbsv_(integer *n, integer *kl, integer *ku, integer *nrhs, complex *ab, integer *ldab, integer *ipiv, complex *b,
    integer *ldb, integer *info);

int cgbsvx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, complex *ab, integer *ldab,
    complex *afb, integer *ldafb, integer *ipiv, char *equed, real *r__, real *c__, complex *b, integer *ldb,
    complex *x, integer *ldx, real *rcond, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int cgbsvxx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, complex *ab, integer *ldab,
    complex *afb, integer *ldafb, integer *ipiv, char *equed, real *r__, real *c__, complex *b, integer *ldb,
    complex *x, integer *ldx, real *rcond, real *rpvgrw, real *berr, integer *n_err_bnds__, real *err_bnds_norm__,
    real *err_bnds_comp__, integer *nparams, real *params, complex *work, real *rwork, integer *info);

int cgbtf2_(integer *m, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, integer *ipiv, integer *info);

int cgbtrf_(integer *m, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, integer *ipiv, integer *info);

int cgbtrs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, complex *ab, integer *ldab, integer *ipiv,
    complex *b, integer *ldb, integer *info);

int cgebak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, real *scale, integer *m, complex *v,
    integer *ldv, integer *info);

int cgebal_(char *job, integer *n, complex *a, integer *lda, integer *ilo, integer *ihi, real *scale, integer *info);

int cgebd2_(integer *m, integer *n, complex *a, integer *lda, real *d__, real *e, complex *tauq, complex *taup,
    complex *work, integer *info);

int cgebrd_(integer *m, integer *n, complex *a, integer *lda, real *d__, real *e, complex *tauq, complex *taup,
    complex *work, integer *lwork, integer *info);

int cgecon_(char *norm, integer *n, complex *a, integer *lda, real *anorm, real *rcond, complex *work, real *rwork,
    integer *info);

int cgeequ_(integer *m, integer *n, complex *a, integer *lda, real *r__, real *c__, real *rowcnd, real *colcnd,
    real *amax, integer *info);

int cgeequb_(integer *m, integer *n, complex *a, integer *lda, real *r__, real *c__, real *rowcnd, real *colcnd,
    real *amax, integer *info);

int cgees_(char *jobvs, char *sort, L_fp select, integer *n, complex *a, integer *lda, integer *sdim, complex *w,
    complex *vs, integer *ldvs, complex *work, integer *lwork, real *rwork, logical *bwork, integer *info);

int cgeesx_(char *jobvs, char *sort, L_fp select, char *sense, integer *n, complex *a, integer *lda, integer *sdim,
    complex *w, complex *vs, integer *ldvs, real *rconde, real *rcondv, complex *work, integer *lwork, real *rwork,
    logical *bwork, integer *info);

int cgeev_(char *jobvl, char *jobvr, integer *n, complex *a, integer *lda, complex *w, complex *vl, integer *ldvl,
    complex *vr, integer *ldvr, complex *work, integer *lwork, real *rwork, integer *info);

int cgeevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, complex *a, integer *lda, complex *w,
    complex *vl, integer *ldvl, complex *vr, integer *ldvr, integer *ilo, integer *ihi, real *scale, real *abnrm,
    real *rconde, real *rcondv, complex *work, integer *lwork, real *rwork, integer *info);

int cgegs_(char *jobvsl, char *jobvsr, integer *n, complex *a, integer *lda, complex *b, integer *ldb, complex *alpha,
    complex *beta, complex *vsl, integer *ldvsl, complex *vsr, integer *ldvsr, complex *work, integer *lwork,
    real *rwork, integer *info);

int cgegv_(char *jobvl, char *jobvr, integer *n, complex *a, integer *lda, complex *b, integer *ldb, complex *alpha,
    complex *beta, complex *vl, integer *ldvl, complex *vr, integer *ldvr, complex *work, integer *lwork, real *rwork,
    integer *info);

int cgehd2_(
    integer *n, integer *ilo, integer *ihi, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cgehrd_(integer *n, integer *ilo, integer *ihi, complex *a, integer *lda, complex *tau, complex *work,
    integer *lwork, integer *info);

int cgelq2_(integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cgelqf_(
    integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *lwork, integer *info);

int cgels_(char *trans, integer *m, integer *n, integer *nrhs, complex *a, integer *lda, complex *b, integer *ldb,
    complex *work, integer *lwork, integer *info);

int cgelsd_(integer *m, integer *n, integer *nrhs, complex *a, integer *lda, complex *b, integer *ldb, real *s,
    real *rcond, integer *rank, complex *work, integer *lwork, real *rwork, integer *iwork, integer *info);

int cgelss_(integer *m, integer *n, integer *nrhs, complex *a, integer *lda, complex *b, integer *ldb, real *s,
    real *rcond, integer *rank, complex *work, integer *lwork, real *rwork, integer *info);

int cgelsx_(integer *m, integer *n, integer *nrhs, complex *a, integer *lda, complex *b, integer *ldb, integer *jpvt,
    real *rcond, integer *rank, complex *work, real *rwork, integer *info);

int cgelsy_(integer *m, integer *n, integer *nrhs, complex *a, integer *lda, complex *b, integer *ldb, integer *jpvt,
    real *rcond, integer *rank, complex *work, integer *lwork, real *rwork, integer *info);

int cgeql2_(integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cgeqlf_(
    integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *lwork, integer *info);

int cgeqp3_(integer *m, integer *n, complex *a, integer *lda, integer *jpvt, complex *tau, complex *work,
    integer *lwork, real *rwork, integer *info);

int cgeqpf_(integer *m, integer *n, complex *a, integer *lda, integer *jpvt, complex *tau, complex *work, real *rwork,
    integer *info);

int cgeqr2_(integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cgeqrf_(
    integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *lwork, integer *info);

int cgerfs_(char *trans, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    complex *b, integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int cgerfsx_(char *trans, char *equed, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, real *r__, real *c__, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *berr,
    integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, complex *work,
    real *rwork, integer *info);

int cgerq2_(integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cgerqf_(
    integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *lwork, integer *info);

int cgesc2_(integer *n, complex *a, integer *lda, complex *rhs, integer *ipiv, integer *jpiv, real *scale);

int cgesdd_(char *jobz, integer *m, integer *n, complex *a, integer *lda, real *s, complex *u, integer *ldu,
    complex *vt, integer *ldvt, complex *work, integer *lwork, real *rwork, integer *iwork, integer *info);

int cgesv_(integer *n, integer *nrhs, complex *a, integer *lda, integer *ipiv, complex *b, integer *ldb, integer *info);

int cgesvd_(char *jobu, char *jobvt, integer *m, integer *n, complex *a, integer *lda, real *s, complex *u,
    integer *ldu, complex *vt, integer *ldvt, complex *work, integer *lwork, real *rwork, integer *info);

int cgesvx_(char *fact, char *trans, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, char *equed, real *r__, real *c__, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond,
    real *ferr, real *berr, complex *work, real *rwork, integer *info);

int cgesvxx_(char *fact, char *trans, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, char *equed, real *r__, real *c__, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond,
    real *rpvgrw, real *berr, integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams,
    real *params, complex *work, real *rwork, integer *info);

int cgetc2_(integer *n, complex *a, integer *lda, integer *ipiv, integer *jpiv, integer *info);

int cgetf2_(integer *m, integer *n, complex *a, integer *lda, integer *ipiv, integer *info);

int cgetrf_(integer *m, integer *n, complex *a, integer *lda, integer *ipiv, integer *info);

int cgetri_(integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *lwork, integer *info);

int cgetrs_(char *trans, integer *n, integer *nrhs, complex *a, integer *lda, integer *ipiv, complex *b, integer *ldb,
    integer *info);

int cggbak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, real *lscale, real *rscale, integer *m,
    complex *v, integer *ldv, integer *info);

int cggbal_(char *job, integer *n, complex *a, integer *lda, complex *b, integer *ldb, integer *ilo, integer *ihi,
    real *lscale, real *rscale, real *work, integer *info);

int cgges_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, integer *n, complex *a, integer *lda, complex *b,
    integer *ldb, integer *sdim, complex *alpha, complex *beta, complex *vsl, integer *ldvsl, complex *vsr,
    integer *ldvsr, complex *work, integer *lwork, real *rwork, logical *bwork, integer *info);

int cggesx_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, char *sense, integer *n, complex *a, integer *lda,
    complex *b, integer *ldb, integer *sdim, complex *alpha, complex *beta, complex *vsl, integer *ldvsl, complex *vsr,
    integer *ldvsr, real *rconde, real *rcondv, complex *work, integer *lwork, real *rwork, integer *iwork,
    integer *liwork, logical *bwork, integer *info);

int cggev_(char *jobvl, char *jobvr, integer *n, complex *a, integer *lda, complex *b, integer *ldb, complex *alpha,
    complex *beta, complex *vl, integer *ldvl, complex *vr, integer *ldvr, complex *work, integer *lwork, real *rwork,
    integer *info);

int cggevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, complex *a, integer *lda, complex *b,
    integer *ldb, complex *alpha, complex *beta, complex *vl, integer *ldvl, complex *vr, integer *ldvr, integer *ilo,
    integer *ihi, real *lscale, real *rscale, real *abnrm, real *bbnrm, real *rconde, real *rcondv, complex *work,
    integer *lwork, real *rwork, integer *iwork, logical *bwork, integer *info);

int cggglm_(integer *n, integer *m, integer *p, complex *a, integer *lda, complex *b, integer *ldb, complex *d__,
    complex *x, complex *y, complex *work, integer *lwork, integer *info);

int cgghrd_(char *compq, char *compz, integer *n, integer *ilo, integer *ihi, complex *a, integer *lda, complex *b,
    integer *ldb, complex *q, integer *ldq, complex *z__, integer *ldz, integer *info);

int cgglse_(integer *m, integer *n, integer *p, complex *a, integer *lda, complex *b, integer *ldb, complex *c__,
    complex *d__, complex *x, complex *work, integer *lwork, integer *info);

int cggqrf_(integer *n, integer *m, integer *p, complex *a, integer *lda, complex *taua, complex *b, integer *ldb,
    complex *taub, complex *work, integer *lwork, integer *info);

int cggrqf_(integer *m, integer *p, integer *n, complex *a, integer *lda, complex *taua, complex *b, integer *ldb,
    complex *taub, complex *work, integer *lwork, integer *info);

int cggsvd_(char *jobu, char *jobv, char *jobq, integer *m, integer *n, integer *p, integer *k, integer *l, complex *a,
    integer *lda, complex *b, integer *ldb, real *alpha, real *beta, complex *u, integer *ldu, complex *v, integer *ldv,
    complex *q, integer *ldq, complex *work, real *rwork, integer *iwork, integer *info);

int cggsvp_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, complex *a, integer *lda,
    complex *b, integer *ldb, real *tola, real *tolb, integer *k, integer *l, complex *u, integer *ldu, complex *v,
    integer *ldv, complex *q, integer *ldq, integer *iwork, real *rwork, complex *tau, complex *work, integer *info);

int cgtcon_(char *norm, integer *n, complex *dl, complex *d__, complex *du, complex *du2, integer *ipiv, real *anorm,
    real *rcond, complex *work, integer *info);

int cgtrfs_(char *trans, integer *n, integer *nrhs, complex *dl, complex *d__, complex *du, complex *dlf, complex *df,
    complex *duf, complex *du2, integer *ipiv, complex *b, integer *ldb, complex *x, integer *ldx, real *ferr,
    real *berr, complex *work, real *rwork, integer *info);

int cgtsv_(integer *n, integer *nrhs, complex *dl, complex *d__, complex *du, complex *b, integer *ldb, integer *info);

int cgtsvx_(char *fact, char *trans, integer *n, integer *nrhs, complex *dl, complex *d__, complex *du, complex *dlf,
    complex *df, complex *duf, complex *du2, integer *ipiv, complex *b, integer *ldb, complex *x, integer *ldx,
    real *rcond, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int cgttrf_(integer *n, complex *dl, complex *d__, complex *du, complex *du2, integer *ipiv, integer *info);

int cgttrs_(char *trans, integer *n, integer *nrhs, complex *dl, complex *d__, complex *du, complex *du2, integer *ipiv,
    complex *b, integer *ldb, integer *info);

int cgtts2_(integer *itrans, integer *n, integer *nrhs, complex *dl, complex *d__, complex *du, complex *du2,
    integer *ipiv, complex *b, integer *ldb);

int chbev_(char *jobz, char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, real *w, complex *z__,
    integer *ldz, complex *work, real *rwork, integer *info);

int chbevd_(char *jobz, char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, real *w, complex *z__,
    integer *ldz, complex *work, integer *lwork, real *rwork, integer *lrwork, integer *iwork, integer *liwork,
    integer *info);

int chbevx_(char *jobz, char *range, char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, complex *q,
    integer *ldq, real *vl, real *vu, integer *il, integer *iu, real *abstol, integer *m, real *w, complex *z__,
    integer *ldz, complex *work, real *rwork, integer *iwork, integer *ifail, integer *info);

int chbgst_(char *vect, char *uplo, integer *n, integer *ka, integer *kb, complex *ab, integer *ldab, complex *bb,
    integer *ldbb, complex *x, integer *ldx, complex *work, real *rwork, integer *info);

int chbgv_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, complex *ab, integer *ldab, complex *bb,
    integer *ldbb, real *w, complex *z__, integer *ldz, complex *work, real *rwork, integer *info);

int chbgvd_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, complex *ab, integer *ldab, complex *bb,
    integer *ldbb, real *w, complex *z__, integer *ldz, complex *work, integer *lwork, real *rwork, integer *lrwork,
    integer *iwork, integer *liwork, integer *info);

int chbgvx_(char *jobz, char *range, char *uplo, integer *n, integer *ka, integer *kb, complex *ab, integer *ldab,
    complex *bb, integer *ldbb, complex *q, integer *ldq, real *vl, real *vu, integer *il, integer *iu, real *abstol,
    integer *m, real *w, complex *z__, integer *ldz, complex *work, real *rwork, integer *iwork, integer *ifail,
    integer *info);

int chbtrd_(char *vect, char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, real *d__, real *e, complex *q,
    integer *ldq, complex *work, integer *info);

int checon_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, real *anorm, real *rcond, complex *work,
    integer *info);

int cheequb_(
    char *uplo, integer *n, complex *a, integer *lda, real *s, real *scond, real *amax, complex *work, integer *info);

int cheev_(char *jobz, char *uplo, integer *n, complex *a, integer *lda, real *w, complex *work, integer *lwork,
    real *rwork, integer *info);

int cheevd_(char *jobz, char *uplo, integer *n, complex *a, integer *lda, real *w, complex *work, integer *lwork,
    real *rwork, integer *lrwork, integer *iwork, integer *liwork, integer *info);

int cheevr_(char *jobz, char *range, char *uplo, integer *n, complex *a, integer *lda, real *vl, real *vu, integer *il,
    integer *iu, real *abstol, integer *m, real *w, complex *z__, integer *ldz, integer *isuppz, complex *work,
    integer *lwork, real *rwork, integer *lrwork, integer *iwork, integer *liwork, integer *info);

int cheevx_(char *jobz, char *range, char *uplo, integer *n, complex *a, integer *lda, real *vl, real *vu, integer *il,
    integer *iu, real *abstol, integer *m, real *w, complex *z__, integer *ldz, complex *work, integer *lwork,
    real *rwork, integer *iwork, integer *ifail, integer *info);

int chegs2_(integer *itype, char *uplo, integer *n, complex *a, integer *lda, complex *b, integer *ldb, integer *info);

int chegst_(integer *itype, char *uplo, integer *n, complex *a, integer *lda, complex *b, integer *ldb, integer *info);

int chegv_(integer *itype, char *jobz, char *uplo, integer *n, complex *a, integer *lda, complex *b, integer *ldb,
    real *w, complex *work, integer *lwork, real *rwork, integer *info);

int chegvd_(integer *itype, char *jobz, char *uplo, integer *n, complex *a, integer *lda, complex *b, integer *ldb,
    real *w, complex *work, integer *lwork, real *rwork, integer *lrwork, integer *iwork, integer *liwork,
    integer *info);

int chegvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, complex *a, integer *lda, complex *b,
    integer *ldb, real *vl, real *vu, integer *il, integer *iu, real *abstol, integer *m, real *w, complex *z__,
    integer *ldz, complex *work, integer *lwork, real *rwork, integer *iwork, integer *ifail, integer *info);

int cherfs_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    complex *b, integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int cherfsx_(char *uplo, char *equed, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *berr,
    integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, complex *work,
    real *rwork, integer *info);

int chesv_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, integer *ipiv, complex *b, integer *ldb,
    complex *work, integer *lwork, integer *info);

int chesvx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr, real *berr,
    complex *work, integer *lwork, real *rwork, integer *info);

int chesvxx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, char *equed, real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *rpvgrw,
    real *berr, integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params,
    complex *work, real *rwork, integer *info);

int chetd2_(char *uplo, integer *n, complex *a, integer *lda, real *d__, real *e, complex *tau, integer *info);

int chetf2_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, integer *info);

int chetrd_(char *uplo, integer *n, complex *a, integer *lda, real *d__, real *e, complex *tau, complex *work,
    integer *lwork, integer *info);

int chetrf_(
    char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *lwork, integer *info);

int chetri_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *info);

int chetrs_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, integer *ipiv, complex *b, integer *ldb,
    integer *info);

int chfrk_(char *transr, char *uplo, char *trans, integer *n, integer *k, real *alpha, complex *a, integer *lda,
    real *beta, complex *c__);

int chgeqz_(char *job, char *compq, char *compz, integer *n, integer *ilo, integer *ihi, complex *h__, integer *ldh,
    complex *t, integer *ldt, complex *alpha, complex *beta, complex *q, integer *ldq, complex *z__, integer *ldz,
    complex *work, integer *lwork, real *rwork, integer *info);

VOID chla_transtype__(char *ret_val, ftnlen ret_val_len, integer *trans);

int chpcon_(char *uplo, integer *n, complex *ap, integer *ipiv, real *anorm, real *rcond, complex *work, integer *info);

int chpev_(char *jobz, char *uplo, integer *n, complex *ap, real *w, complex *z__, integer *ldz, complex *work,
    real *rwork, integer *info);

int chpevd_(char *jobz, char *uplo, integer *n, complex *ap, real *w, complex *z__, integer *ldz, complex *work,
    integer *lwork, real *rwork, integer *lrwork, integer *iwork, integer *liwork, integer *info);

int chpevx_(char *jobz, char *range, char *uplo, integer *n, complex *ap, real *vl, real *vu, integer *il, integer *iu,
    real *abstol, integer *m, real *w, complex *z__, integer *ldz, complex *work, real *rwork, integer *iwork,
    integer *ifail, integer *info);

int chpgst_(integer *itype, char *uplo, integer *n, complex *ap, complex *bp, integer *info);

int chpgv_(integer *itype, char *jobz, char *uplo, integer *n, complex *ap, complex *bp, real *w, complex *z__,
    integer *ldz, complex *work, real *rwork, integer *info);

int chpgvd_(integer *itype, char *jobz, char *uplo, integer *n, complex *ap, complex *bp, real *w, complex *z__,
    integer *ldz, complex *work, integer *lwork, real *rwork, integer *lrwork, integer *iwork, integer *liwork,
    integer *info);

int chpgvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, complex *ap, complex *bp, real *vl,
    real *vu, integer *il, integer *iu, real *abstol, integer *m, real *w, complex *z__, integer *ldz, complex *work,
    real *rwork, integer *iwork, integer *ifail, integer *info);

int chprfs_(char *uplo, integer *n, integer *nrhs, complex *ap, complex *afp, integer *ipiv, complex *b, integer *ldb,
    complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int chpsv_(char *uplo, integer *n, integer *nrhs, complex *ap, integer *ipiv, complex *b, integer *ldb, integer *info);

int chpsvx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *ap, complex *afp, integer *ipiv, complex *b,
    integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int chptrd_(char *uplo, integer *n, complex *ap, real *d__, real *e, complex *tau, integer *info);

int chptrf_(char *uplo, integer *n, complex *ap, integer *ipiv, integer *info);

int chptri_(char *uplo, integer *n, complex *ap, integer *ipiv, complex *work, integer *info);

int chptrs_(char *uplo, integer *n, integer *nrhs, complex *ap, integer *ipiv, complex *b, integer *ldb, integer *info);

int chsein_(char *side, char *eigsrc, char *initv, logical *select, integer *n, complex *h__, integer *ldh, complex *w,
    complex *vl, integer *ldvl, complex *vr, integer *ldvr, integer *mm, integer *m, complex *work, real *rwork,
    integer *ifaill, integer *ifailr, integer *info);

int chseqr_(char *job, char *compz, integer *n, integer *ilo, integer *ihi, complex *h__, integer *ldh, complex *w,
    complex *z__, integer *ldz, complex *work, integer *lwork, integer *info);

int cla_gbamv__(integer *trans, integer *m, integer *n, integer *kl, integer *ku, real *alpha, complex *ab,
    integer *ldab, complex *x, integer *incx, real *beta, real *y, integer *incy);

doublereal cla_gbrcond_c__(char *trans, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, complex *afb,
    integer *ldafb, integer *ipiv, real *c__, logical *capply, integer *info, complex *work, real *rwork,
    ftnlen trans_len);

doublereal cla_gbrcond_x__(char *trans, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, complex *afb,
    integer *ldafb, integer *ipiv, complex *x, integer *info, complex *work, real *rwork, ftnlen trans_len);

int cla_gbrfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *kl, integer *ku,
    integer *nrhs, complex *ab, integer *ldab, complex *afb, integer *ldafb, integer *ipiv, logical *colequ, real *c__,
    complex *b, integer *ldb, complex *y, integer *ldy, real *berr_out__, integer *n_norms__, real *errs_n__,
    real *errs_c__, complex *res, real *ayb, complex *dy, complex *y_tail__, real *rcond, integer *ithresh,
    real *rthresh, real *dz_ub__, logical *ignore_cwise__, integer *info);

doublereal cla_gbrpvgrw__(
    integer *n, integer *kl, integer *ku, integer *ncols, complex *ab, integer *ldab, complex *afb, integer *ldafb);

int cla_geamv__(integer *trans, integer *m, integer *n, real *alpha, complex *a, integer *lda, complex *x,
    integer *incx, real *beta, real *y, integer *incy);

doublereal cla_gercond_c__(char *trans, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    real *c__, logical *capply, integer *info, complex *work, real *rwork, ftnlen trans_len);

doublereal cla_gercond_x__(char *trans, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    complex *x, integer *info, complex *work, real *rwork, ftnlen trans_len);

int cla_gerfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *nrhs, complex *a,
    integer *lda, complex *af, integer *ldaf, integer *ipiv, logical *colequ, real *c__, complex *b, integer *ldb,
    complex *y, integer *ldy, real *berr_out__, integer *n_norms__, real *errs_n__, real *errs_c__, complex *res,
    real *ayb, complex *dy, complex *y_tail__, real *rcond, integer *ithresh, real *rthresh, real *dz_ub__,
    logical *ignore_cwise__, integer *info);

int cla_heamv__(integer *uplo, integer *n, real *alpha, complex *a, integer *lda, complex *x, integer *incx, real *beta,
    real *y, integer *incy);

doublereal cla_hercond_c__(char *uplo, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    real *c__, logical *capply, integer *info, complex *work, real *rwork, ftnlen uplo_len);

doublereal cla_hercond_x__(char *uplo, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    complex *x, integer *info, complex *work, real *rwork, ftnlen uplo_len);

int cla_herfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda,
    complex *af, integer *ldaf, integer *ipiv, logical *colequ, real *c__, complex *b, integer *ldb, complex *y,
    integer *ldy, real *berr_out__, integer *n_norms__, real *errs_n__, real *errs_c__, complex *res, real *ayb,
    complex *dy, complex *y_tail__, real *rcond, integer *ithresh, real *rthresh, real *dz_ub__,
    logical *ignore_cwise__, integer *info, ftnlen uplo_len);

doublereal cla_herpvgrw__(char *uplo, integer *n, integer *info, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, real *work, ftnlen uplo_len);

int cla_lin_berr__(integer *n, integer *nz, integer *nrhs, complex *res, real *ayb, real *berr);

doublereal cla_porcond_c__(char *uplo, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, real *c__,
    logical *capply, integer *info, complex *work, real *rwork, ftnlen uplo_len);

doublereal cla_porcond_x__(char *uplo, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, complex *x,
    integer *info, complex *work, real *rwork, ftnlen uplo_len);

int cla_porfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda,
    complex *af, integer *ldaf, logical *colequ, real *c__, complex *b, integer *ldb, complex *y, integer *ldy,
    real *berr_out__, integer *n_norms__, real *errs_n__, real *errs_c__, complex *res, real *ayb, complex *dy,
    complex *y_tail__, real *rcond, integer *ithresh, real *rthresh, real *dz_ub__, logical *ignore_cwise__,
    integer *info, ftnlen uplo_len);

doublereal cla_porpvgrw__(
    char *uplo, integer *ncols, complex *a, integer *lda, complex *af, integer *ldaf, real *work, ftnlen uplo_len);

doublereal cla_rpvgrw__(integer *n, integer *ncols, complex *a, integer *lda, complex *af, integer *ldaf);

int cla_syamv__(integer *uplo, integer *n, real *alpha, complex *a, integer *lda, complex *x, integer *incx, real *beta,
    real *y, integer *incy);

doublereal cla_syrcond_c__(char *uplo, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    real *c__, logical *capply, integer *info, complex *work, real *rwork, ftnlen uplo_len);

doublereal cla_syrcond_x__(char *uplo, integer *n, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    complex *x, integer *info, complex *work, real *rwork, ftnlen uplo_len);

int cla_syrfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda,
    complex *af, integer *ldaf, integer *ipiv, logical *colequ, real *c__, complex *b, integer *ldb, complex *y,
    integer *ldy, real *berr_out__, integer *n_norms__, real *errs_n__, real *errs_c__, complex *res, real *ayb,
    complex *dy, complex *y_tail__, real *rcond, integer *ithresh, real *rthresh, real *dz_ub__,
    logical *ignore_cwise__, integer *info, ftnlen uplo_len);

doublereal cla_syrpvgrw__(char *uplo, integer *n, integer *info, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, real *work, ftnlen uplo_len);

int cla_wwaddw__(integer *n, complex *x, complex *y, complex *w);

int clabrd_(integer *m, integer *n, integer *nb, complex *a, integer *lda, real *d__, real *e, complex *tauq,
    complex *taup, complex *x, integer *ldx, complex *y, integer *ldy);

int clacgv_(integer *n, complex *x, integer *incx);

int clacn2_(integer *n, complex *v, complex *x, real *est, integer *kase, integer *isave);

int clacon_(integer *n, complex *v, complex *x, real *est, integer *kase);

int clacp2_(char *uplo, integer *m, integer *n, real *a, integer *lda, complex *b, integer *ldb);

int clacpy_(char *uplo, integer *m, integer *n, complex *a, integer *lda, complex *b, integer *ldb);

int clacrm_(
    integer *m, integer *n, complex *a, integer *lda, real *b, integer *ldb, complex *c__, integer *ldc, real *rwork);

int clacrt_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy, complex *c__, complex *s);

VOID cladiv_(complex *ret_val, complex *x, complex *y);

int claed0_(integer *qsiz, integer *n, real *d__, real *e, complex *q, integer *ldq, complex *qstore, integer *ldqs,
    real *rwork, integer *iwork, integer *info);

int claed7_(integer *n, integer *cutpnt, integer *qsiz, integer *tlvls, integer *curlvl, integer *curpbm, real *d__,
    complex *q, integer *ldq, real *rho, integer *indxq, real *qstore, integer *qptr, integer *prmptr, integer *perm,
    integer *givptr, integer *givcol, real *givnum, complex *work, real *rwork, integer *iwork, integer *info);

int claed8_(integer *k, integer *n, integer *qsiz, complex *q, integer *ldq, real *d__, real *rho, integer *cutpnt,
    real *z__, real *dlamda, complex *q2, integer *ldq2, real *w, integer *indxp, integer *indx, integer *indxq,
    integer *perm, integer *givptr, integer *givcol, real *givnum, integer *info);

int claein_(logical *rightv, logical *noinit, integer *n, complex *h__, integer *ldh, complex *w, complex *v,
    complex *b, integer *ldb, real *rwork, real *eps3, real *smlnum, integer *info);

int claesy_(
    complex *a, complex *b, complex *c__, complex *rt1, complex *rt2, complex *evscal, complex *cs1, complex *sn1);

int claev2_(complex *a, complex *b, complex *c__, real *rt1, real *rt2, real *cs1, complex *sn1);

int clag2z_(integer *m, integer *n, complex *sa, integer *ldsa, doublecomplex *a, integer *lda, integer *info);

int clags2_(logical *upper, real *a1, complex *a2, real *a3, real *b1, complex *b2, real *b3, real *csu, complex *snu,
    real *csv, complex *snv, real *csq, complex *snq);

int clagtm_(char *trans, integer *n, integer *nrhs, real *alpha, complex *dl, complex *d__, complex *du, complex *x,
    integer *ldx, real *beta, complex *b, integer *ldb);

int clahef_(char *uplo, integer *n, integer *nb, integer *kb, complex *a, integer *lda, integer *ipiv, complex *w,
    integer *ldw, integer *info);

int clahqr_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, complex *h__, integer *ldh,
    complex *w, integer *iloz, integer *ihiz, complex *z__, integer *ldz, integer *info);

int clahr2_(integer *n, integer *k, integer *nb, complex *a, integer *lda, complex *tau, complex *t, integer *ldt,
    complex *y, integer *ldy);

int clahrd_(integer *n, integer *k, integer *nb, complex *a, integer *lda, complex *tau, complex *t, integer *ldt,
    complex *y, integer *ldy);

int claic1_(integer *job, integer *j, complex *x, real *sest, complex *w, complex *gamma, real *sestpr, complex *s,
    complex *c__);

int clals0_(integer *icompq, integer *nl, integer *nr, integer *sqre, integer *nrhs, complex *b, integer *ldb,
    complex *bx, integer *ldbx, integer *perm, integer *givptr, integer *givcol, integer *ldgcol, real *givnum,
    integer *ldgnum, real *poles, real *difl, real *difr, real *z__, integer *k, real *c__, real *s, real *rwork,
    integer *info);

int clalsa_(integer *icompq, integer *smlsiz, integer *n, integer *nrhs, complex *b, integer *ldb, complex *bx,
    integer *ldbx, real *u, integer *ldu, real *vt, integer *k, real *difl, real *difr, real *z__, real *poles,
    integer *givptr, integer *givcol, integer *ldgcol, integer *perm, real *givnum, real *c__, real *s, real *rwork,
    integer *iwork, integer *info);

int clalsd_(char *uplo, integer *smlsiz, integer *n, integer *nrhs, real *d__, real *e, complex *b, integer *ldb,
    real *rcond, integer *rank, complex *work, real *rwork, integer *iwork, integer *info);

doublereal clangb_(char *norm, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, real *work);

doublereal clange_(char *norm, integer *m, integer *n, complex *a, integer *lda, real *work);

doublereal clangt_(char *norm, integer *n, complex *dl, complex *d__, complex *du);

doublereal clanhb_(char *norm, char *uplo, integer *n, integer *k, complex *ab, integer *ldab, real *work);

doublereal clanhe_(char *norm, char *uplo, integer *n, complex *a, integer *lda, real *work);

doublereal clanhf_(char *norm, char *transr, char *uplo, integer *n, complex *a, real *work);

doublereal clanhp_(char *norm, char *uplo, integer *n, complex *ap, real *work);

doublereal clanhs_(char *norm, integer *n, complex *a, integer *lda, real *work);

doublereal clanht_(char *norm, integer *n, real *d__, complex *e);

doublereal clansb_(char *norm, char *uplo, integer *n, integer *k, complex *ab, integer *ldab, real *work);

doublereal clansp_(char *norm, char *uplo, integer *n, complex *ap, real *work);

doublereal clansy_(char *norm, char *uplo, integer *n, complex *a, integer *lda, real *work);

doublereal clantb_(char *norm, char *uplo, char *diag, integer *n, integer *k, complex *ab, integer *ldab, real *work);

doublereal clantp_(char *norm, char *uplo, char *diag, integer *n, complex *ap, real *work);

doublereal clantr_(char *norm, char *uplo, char *diag, integer *m, integer *n, complex *a, integer *lda, real *work);

int clapll_(integer *n, complex *x, integer *incx, complex *y, integer *incy, real *ssmin);

int clapmt_(logical *forwrd, integer *m, integer *n, complex *x, integer *ldx, integer *k);

int claqgb_(integer *m, integer *n, integer *kl, integer *ku, complex *ab, integer *ldab, real *r__, real *c__,
    real *rowcnd, real *colcnd, real *amax, char *equed);

int claqge_(integer *m, integer *n, complex *a, integer *lda, real *r__, real *c__, real *rowcnd, real *colcnd,
    real *amax, char *equed);

int claqhb_(
    char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, real *s, real *scond, real *amax, char *equed);

int claqhe_(char *uplo, integer *n, complex *a, integer *lda, real *s, real *scond, real *amax, char *equed);

int claqhp_(char *uplo, integer *n, complex *ap, real *s, real *scond, real *amax, char *equed);

int claqp2_(integer *m, integer *n, integer *offset, complex *a, integer *lda, integer *jpvt, complex *tau, real *vn1,
    real *vn2, complex *work);

int claqps_(integer *m, integer *n, integer *offset, integer *nb, integer *kb, complex *a, integer *lda, integer *jpvt,
    complex *tau, real *vn1, real *vn2, complex *auxv, complex *f, integer *ldf);

int claqr0_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, complex *h__, integer *ldh,
    complex *w, integer *iloz, integer *ihiz, complex *z__, integer *ldz, complex *work, integer *lwork, integer *info);

int claqr1_(integer *n, complex *h__, integer *ldh, complex *s1, complex *s2, complex *v);

int claqr2_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, complex *h__,
    integer *ldh, integer *iloz, integer *ihiz, complex *z__, integer *ldz, integer *ns, integer *nd, complex *sh,
    complex *v, integer *ldv, integer *nh, complex *t, integer *ldt, integer *nv, complex *wv, integer *ldwv,
    complex *work, integer *lwork);

int claqr3_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, complex *h__,
    integer *ldh, integer *iloz, integer *ihiz, complex *z__, integer *ldz, integer *ns, integer *nd, complex *sh,
    complex *v, integer *ldv, integer *nh, complex *t, integer *ldt, integer *nv, complex *wv, integer *ldwv,
    complex *work, integer *lwork);

int claqr4_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, complex *h__, integer *ldh,
    complex *w, integer *iloz, integer *ihiz, complex *z__, integer *ldz, complex *work, integer *lwork, integer *info);

int claqr5_(logical *wantt, logical *wantz, integer *kacc22, integer *n, integer *ktop, integer *kbot, integer *nshfts,
    complex *s, complex *h__, integer *ldh, integer *iloz, integer *ihiz, complex *z__, integer *ldz, complex *v,
    integer *ldv, complex *u, integer *ldu, integer *nv, complex *wv, integer *ldwv, integer *nh, complex *wh,
    integer *ldwh);

int claqsb_(
    char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, real *s, real *scond, real *amax, char *equed);

int claqsp_(char *uplo, integer *n, complex *ap, real *s, real *scond, real *amax, char *equed);

int claqsy_(char *uplo, integer *n, complex *a, integer *lda, real *s, real *scond, real *amax, char *equed);

int clar1v_(integer *n, integer *b1, integer *bn, real *lambda, real *d__, real *l, real *ld, real *lld, real *pivmin,
    real *gaptol, complex *z__, logical *wantnc, integer *negcnt, real *ztz, real *mingma, integer *r__,
    integer *isuppz, real *nrminv, real *resid, real *rqcorr, real *work);

int clar2v_(integer *n, complex *x, complex *y, complex *z__, integer *incx, real *c__, complex *s, integer *incc);

int clarcm_(
    integer *m, integer *n, real *a, integer *lda, complex *b, integer *ldb, complex *c__, integer *ldc, real *rwork);

int clarf_(char *side, integer *m, integer *n, complex *v, integer *incv, complex *tau, complex *c__, integer *ldc,
    complex *work);

int clarfb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, complex *v,
    integer *ldv, complex *t, integer *ldt, complex *c__, integer *ldc, complex *work, integer *ldwork);

int clarfg_(integer *n, complex *alpha, complex *x, integer *incx, complex *tau);

int clarfp_(integer *n, complex *alpha, complex *x, integer *incx, complex *tau);

int clarft_(char *direct, char *storev, integer *n, integer *k, complex *v, integer *ldv, complex *tau, complex *t,
    integer *ldt);

int clarfx_(char *side, integer *m, integer *n, complex *v, complex *tau, complex *c__, integer *ldc, complex *work);

int clargv_(integer *n, complex *x, integer *incx, complex *y, integer *incy, real *c__, integer *incc);

int clarnv_(integer *idist, integer *iseed, integer *n, complex *x);

int clarrv_(integer *n, real *vl, real *vu, real *d__, real *l, real *pivmin, integer *isplit, integer *m, integer *dol,
    integer *dou, real *minrgp, real *rtol1, real *rtol2, real *w, real *werr, real *wgap, integer *iblock,
    integer *indexw, real *gers, complex *z__, integer *ldz, integer *isuppz, real *work, integer *iwork,
    integer *info);

int clarscl2_(integer *m, integer *n, real *d__, complex *x, integer *ldx);

int clartg_(complex *f, complex *g, real *cs, complex *sn, complex *r__);

int clartv_(integer *n, complex *x, integer *incx, complex *y, integer *incy, real *c__, complex *s, integer *incc);

int clarz_(char *side, integer *m, integer *n, integer *l, complex *v, integer *incv, complex *tau, complex *c__,
    integer *ldc, complex *work);

int clarzb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, integer *l,
    complex *v, integer *ldv, complex *t, integer *ldt, complex *c__, integer *ldc, complex *work, integer *ldwork);

int clarzt_(char *direct, char *storev, integer *n, integer *k, complex *v, integer *ldv, complex *tau, complex *t,
    integer *ldt);

int clascl_(char *type__, integer *kl, integer *ku, real *cfrom, real *cto, integer *m, integer *n, complex *a,
    integer *lda, integer *info);

int clascl2_(integer *m, integer *n, real *d__, complex *x, integer *ldx);

int claset_(char *uplo, integer *m, integer *n, complex *alpha, complex *beta, complex *a, integer *lda);

int clasr_(char *side, char *pivot, char *direct, integer *m, integer *n, real *c__, real *s, complex *a, integer *lda);

int classq_(integer *n, complex *x, integer *incx, real *scale, real *sumsq);

int claswp_(integer *n, complex *a, integer *lda, integer *k1, integer *k2, integer *ipiv, integer *incx);

int clasyf_(char *uplo, integer *n, integer *nb, integer *kb, complex *a, integer *lda, integer *ipiv, complex *w,
    integer *ldw, integer *info);

int clatbs_(char *uplo, char *trans, char *diag, char *normin, integer *n, integer *kd, complex *ab, integer *ldab,
    complex *x, real *scale, real *cnorm, integer *info);

int clatdf_(integer *ijob, integer *n, complex *z__, integer *ldz, complex *rhs, real *rdsum, real *rdscal,
    integer *ipiv, integer *jpiv);

int clatps_(char *uplo, char *trans, char *diag, char *normin, integer *n, complex *ap, complex *x, real *scale,
    real *cnorm, integer *info);

int clatrd_(
    char *uplo, integer *n, integer *nb, complex *a, integer *lda, real *e, complex *tau, complex *w, integer *ldw);

int clatrs_(char *uplo, char *trans, char *diag, char *normin, integer *n, complex *a, integer *lda, complex *x,
    real *scale, real *cnorm, integer *info);

int clatrz_(integer *m, integer *n, integer *l, complex *a, integer *lda, complex *tau, complex *work);

int clatzm_(char *side, integer *m, integer *n, complex *v, integer *incv, complex *tau, complex *c1, complex *c2,
    integer *ldc, complex *work);

int clauu2_(char *uplo, integer *n, complex *a, integer *lda, integer *info);

int clauum_(char *uplo, integer *n, complex *a, integer *lda, integer *info);

int cpbcon_(char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, real *anorm, real *rcond, complex *work,
    real *rwork, integer *info);

int cpbequ_(
    char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, real *s, real *scond, real *amax, integer *info);

int cpbrfs_(char *uplo, integer *n, integer *kd, integer *nrhs, complex *ab, integer *ldab, complex *afb,
    integer *ldafb, complex *b, integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work,
    real *rwork, integer *info);

int cpbstf_(char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, integer *info);

int cpbsv_(char *uplo, integer *n, integer *kd, integer *nrhs, complex *ab, integer *ldab, complex *b, integer *ldb,
    integer *info);

int cpbsvx_(char *fact, char *uplo, integer *n, integer *kd, integer *nrhs, complex *ab, integer *ldab, complex *afb,
    integer *ldafb, char *equed, real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr,
    real *berr, complex *work, real *rwork, integer *info);

int cpbtf2_(char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, integer *info);

int cpbtrf_(char *uplo, integer *n, integer *kd, complex *ab, integer *ldab, integer *info);

int cpbtrs_(char *uplo, integer *n, integer *kd, integer *nrhs, complex *ab, integer *ldab, complex *b, integer *ldb,
    integer *info);

int cpftrf_(char *transr, char *uplo, integer *n, complex *a, integer *info);

int cpftri_(char *transr, char *uplo, integer *n, complex *a, integer *info);

int cpftrs_(char *transr, char *uplo, integer *n, integer *nrhs, complex *a, complex *b, integer *ldb, integer *info);

int cpocon_(char *uplo, integer *n, complex *a, integer *lda, real *anorm, real *rcond, complex *work, real *rwork,
    integer *info);

int cpoequ_(integer *n, complex *a, integer *lda, real *s, real *scond, real *amax, integer *info);

int cpoequb_(integer *n, complex *a, integer *lda, real *s, real *scond, real *amax, integer *info);

int cporfs_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf, complex *b,
    integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int cporfsx_(char *uplo, char *equed, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *berr, integer *n_err_bnds__,
    real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, complex *work, real *rwork,
    integer *info);

int cposv_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *b, integer *ldb, integer *info);

int cposvx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    char *equed, real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr, real *berr,
    complex *work, real *rwork, integer *info);

int cposvxx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    char *equed, real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *rpvgrw, real *berr,
    integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, complex *work,
    real *rwork, integer *info);

int cpotf2_(char *uplo, integer *n, complex *a, integer *lda, integer *info);

int cpotrf_(char *uplo, integer *n, complex *a, integer *lda, integer *info);

int cpotri_(char *uplo, integer *n, complex *a, integer *lda, integer *info);

int cpotrs_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *b, integer *ldb, integer *info);

int cppcon_(char *uplo, integer *n, complex *ap, real *anorm, real *rcond, complex *work, real *rwork, integer *info);

int cppequ_(char *uplo, integer *n, complex *ap, real *s, real *scond, real *amax, integer *info);

int cpprfs_(char *uplo, integer *n, integer *nrhs, complex *ap, complex *afp, complex *b, integer *ldb, complex *x,
    integer *ldx, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int cppsv_(char *uplo, integer *n, integer *nrhs, complex *ap, complex *b, integer *ldb, integer *info);

int cppsvx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *ap, complex *afp, char *equed, real *s,
    complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int cpptrf_(char *uplo, integer *n, complex *ap, integer *info);

int cpptri_(char *uplo, integer *n, complex *ap, integer *info);

int cpptrs_(char *uplo, integer *n, integer *nrhs, complex *ap, complex *b, integer *ldb, integer *info);

int cpstf2_(char *uplo, integer *n, complex *a, integer *lda, integer *piv, integer *rank, real *tol, real *work,
    integer *info);

int cpstrf_(char *uplo, integer *n, complex *a, integer *lda, integer *piv, integer *rank, real *tol, real *work,
    integer *info);

int cptcon_(integer *n, real *d__, complex *e, real *anorm, real *rcond, real *rwork, integer *info);

int cpteqr_(char *compz, integer *n, real *d__, real *e, complex *z__, integer *ldz, real *work, integer *info);

int cptrfs_(char *uplo, integer *n, integer *nrhs, real *d__, complex *e, real *df, complex *ef, complex *b,
    integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int cptsv_(integer *n, integer *nrhs, real *d__, complex *e, complex *b, integer *ldb, integer *info);

int cptsvx_(char *fact, integer *n, integer *nrhs, real *d__, complex *e, real *df, complex *ef, complex *b,
    integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int cpttrf_(integer *n, real *d__, complex *e, integer *info);

int cpttrs_(char *uplo, integer *n, integer *nrhs, real *d__, complex *e, complex *b, integer *ldb, integer *info);

int cptts2_(integer *iuplo, integer *n, integer *nrhs, real *d__, complex *e, complex *b, integer *ldb);

int crot_(integer *n, complex *cx, integer *incx, complex *cy, integer *incy, real *c__, complex *s);

int cspcon_(char *uplo, integer *n, complex *ap, integer *ipiv, real *anorm, real *rcond, complex *work, integer *info);

int cspmv_(char *uplo, integer *n, complex *alpha, complex *ap, complex *x, integer *incx, complex *beta, complex *y,
    integer *incy);

int cspr_(char *uplo, integer *n, complex *alpha, complex *x, integer *incx, complex *ap);

int csprfs_(char *uplo, integer *n, integer *nrhs, complex *ap, complex *afp, integer *ipiv, complex *b, integer *ldb,
    complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int cspsv_(char *uplo, integer *n, integer *nrhs, complex *ap, integer *ipiv, complex *b, integer *ldb, integer *info);

int cspsvx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *ap, complex *afp, integer *ipiv, complex *b,
    integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int csptrf_(char *uplo, integer *n, complex *ap, integer *ipiv, integer *info);

int csptri_(char *uplo, integer *n, complex *ap, integer *ipiv, complex *work, integer *info);

int csptrs_(char *uplo, integer *n, integer *nrhs, complex *ap, integer *ipiv, complex *b, integer *ldb, integer *info);

int csrscl_(integer *n, real *sa, complex *sx, integer *incx);

int cstedc_(char *compz, integer *n, real *d__, real *e, complex *z__, integer *ldz, complex *work, integer *lwork,
    real *rwork, integer *lrwork, integer *iwork, integer *liwork, integer *info);

int cstegr_(char *jobz, char *range, integer *n, real *d__, real *e, real *vl, real *vu, integer *il, integer *iu,
    real *abstol, integer *m, real *w, complex *z__, integer *ldz, integer *isuppz, real *work, integer *lwork,
    integer *iwork, integer *liwork, integer *info);

int cstein_(integer *n, real *d__, real *e, integer *m, real *w, integer *iblock, integer *isplit, complex *z__,
    integer *ldz, real *work, integer *iwork, integer *ifail, integer *info);

int cstemr_(char *jobz, char *range, integer *n, real *d__, real *e, real *vl, real *vu, integer *il, integer *iu,
    integer *m, real *w, complex *z__, integer *ldz, integer *nzc, integer *isuppz, logical *tryrac, real *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int csteqr_(char *compz, integer *n, real *d__, real *e, complex *z__, integer *ldz, real *work, integer *info);

int csycon_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, real *anorm, real *rcond, complex *work,
    integer *info);

int csyequb_(
    char *uplo, integer *n, complex *a, integer *lda, real *s, real *scond, real *amax, complex *work, integer *info);

int csymv_(char *uplo, integer *n, complex *alpha, complex *a, integer *lda, complex *x, integer *incx, complex *beta,
    complex *y, integer *incy);

int csyr_(char *uplo, integer *n, complex *alpha, complex *x, integer *incx, complex *a, integer *lda);

int csyrfs_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf, integer *ipiv,
    complex *b, integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int csyrfsx_(char *uplo, char *equed, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *berr,
    integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, complex *work,
    real *rwork, integer *info);

int csysv_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, integer *ipiv, complex *b, integer *ldb,
    complex *work, integer *lwork, integer *info);

int csysvx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *ferr, real *berr,
    complex *work, integer *lwork, real *rwork, integer *info);

int csysvxx_(char *fact, char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, complex *af, integer *ldaf,
    integer *ipiv, char *equed, real *s, complex *b, integer *ldb, complex *x, integer *ldx, real *rcond, real *rpvgrw,
    real *berr, integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params,
    complex *work, real *rwork, integer *info);

int csytf2_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, integer *info);

int csytrf_(
    char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *lwork, integer *info);

int csytri_(char *uplo, integer *n, complex *a, integer *lda, integer *ipiv, complex *work, integer *info);

int csytrs_(char *uplo, integer *n, integer *nrhs, complex *a, integer *lda, integer *ipiv, complex *b, integer *ldb,
    integer *info);

int ctbcon_(char *norm, char *uplo, char *diag, integer *n, integer *kd, complex *ab, integer *ldab, real *rcond,
    complex *work, real *rwork, integer *info);

int ctbrfs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, complex *ab, integer *ldab,
    complex *b, integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork,
    integer *info);

int ctbtrs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, complex *ab, integer *ldab,
    complex *b, integer *ldb, integer *info);

int ctfsm_(char *transr, char *side, char *uplo, char *trans, char *diag, integer *m, integer *n, complex *alpha,
    complex *a, complex *b, integer *ldb);

int ctftri_(char *transr, char *uplo, char *diag, integer *n, complex *a, integer *info);

int ctfttp_(char *transr, char *uplo, integer *n, complex *arf, complex *ap, integer *info);

int ctfttr_(char *transr, char *uplo, integer *n, complex *arf, complex *a, integer *lda, integer *info);

int ctgevc_(char *side, char *howmny, logical *select, integer *n, complex *s, integer *lds, complex *p, integer *ldp,
    complex *vl, integer *ldvl, complex *vr, integer *ldvr, integer *mm, integer *m, complex *work, real *rwork,
    integer *info);

int ctgex2_(logical *wantq, logical *wantz, integer *n, complex *a, integer *lda, complex *b, integer *ldb, complex *q,
    integer *ldq, complex *z__, integer *ldz, integer *j1, integer *info);

int ctgexc_(logical *wantq, logical *wantz, integer *n, complex *a, integer *lda, complex *b, integer *ldb, complex *q,
    integer *ldq, complex *z__, integer *ldz, integer *ifst, integer *ilst, integer *info);

int ctgsen_(integer *ijob, logical *wantq, logical *wantz, logical *select, integer *n, complex *a, integer *lda,
    complex *b, integer *ldb, complex *alpha, complex *beta, complex *q, integer *ldq, complex *z__, integer *ldz,
    integer *m, real *pl, real *pr, real *dif, complex *work, integer *lwork, integer *iwork, integer *liwork,
    integer *info);

int ctgsja_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, integer *k, integer *l, complex *a,
    integer *lda, complex *b, integer *ldb, real *tola, real *tolb, real *alpha, real *beta, complex *u, integer *ldu,
    complex *v, integer *ldv, complex *q, integer *ldq, complex *work, integer *ncycle, integer *info);

int ctgsna_(char *job, char *howmny, logical *select, integer *n, complex *a, integer *lda, complex *b, integer *ldb,
    complex *vl, integer *ldvl, complex *vr, integer *ldvr, real *s, real *dif, integer *mm, integer *m, complex *work,
    integer *lwork, integer *iwork, integer *info);

int ctgsy2_(char *trans, integer *ijob, integer *m, integer *n, complex *a, integer *lda, complex *b, integer *ldb,
    complex *c__, integer *ldc, complex *d__, integer *ldd, complex *e, integer *lde, complex *f, integer *ldf,
    real *scale, real *rdsum, real *rdscal, integer *info);

int ctgsyl_(char *trans, integer *ijob, integer *m, integer *n, complex *a, integer *lda, complex *b, integer *ldb,
    complex *c__, integer *ldc, complex *d__, integer *ldd, complex *e, integer *lde, complex *f, integer *ldf,
    real *scale, real *dif, complex *work, integer *lwork, integer *iwork, integer *info);

int ctpcon_(char *norm, char *uplo, char *diag, integer *n, complex *ap, real *rcond, complex *work, real *rwork,
    integer *info);

int ctprfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, complex *ap, complex *b, integer *ldb,
    complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int ctptri_(char *uplo, char *diag, integer *n, complex *ap, integer *info);

int ctptrs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, complex *ap, complex *b, integer *ldb,
    integer *info);

int ctpttf_(char *transr, char *uplo, integer *n, complex *ap, complex *arf, integer *info);

int ctpttr_(char *uplo, integer *n, complex *ap, complex *a, integer *lda, integer *info);

int ctrcon_(char *norm, char *uplo, char *diag, integer *n, complex *a, integer *lda, real *rcond, complex *work,
    real *rwork, integer *info);

int ctrevc_(char *side, char *howmny, logical *select, integer *n, complex *t, integer *ldt, complex *vl, integer *ldvl,
    complex *vr, integer *ldvr, integer *mm, integer *m, complex *work, real *rwork, integer *info);

int ctrexc_(char *compq, integer *n, complex *t, integer *ldt, complex *q, integer *ldq, integer *ifst, integer *ilst,
    integer *info);

int ctrrfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, complex *a, integer *lda, complex *b,
    integer *ldb, complex *x, integer *ldx, real *ferr, real *berr, complex *work, real *rwork, integer *info);

int ctrsen_(char *job, char *compq, logical *select, integer *n, complex *t, integer *ldt, complex *q, integer *ldq,
    complex *w, integer *m, real *s, real *sep, complex *work, integer *lwork, integer *info);

int ctrsna_(char *job, char *howmny, logical *select, integer *n, complex *t, integer *ldt, complex *vl, integer *ldvl,
    complex *vr, integer *ldvr, real *s, real *sep, integer *mm, integer *m, complex *work, integer *ldwork,
    real *rwork, integer *info);

int ctrsyl_(char *trana, char *tranb, integer *isgn, integer *m, integer *n, complex *a, integer *lda, complex *b,
    integer *ldb, complex *c__, integer *ldc, real *scale, integer *info);

int ctrti2_(char *uplo, char *diag, integer *n, complex *a, integer *lda, integer *info);

int ctrtri_(char *uplo, char *diag, integer *n, complex *a, integer *lda, integer *info);

int ctrtrs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, complex *a, integer *lda, complex *b,
    integer *ldb, integer *info);

int ctrttf_(char *transr, char *uplo, integer *n, complex *a, integer *lda, complex *arf, integer *info);

int ctrttp_(char *uplo, integer *n, complex *a, integer *lda, complex *ap, integer *info);

int ctzrqf_(integer *m, integer *n, complex *a, integer *lda, complex *tau, integer *info);

int ctzrzf_(
    integer *m, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *lwork, integer *info);

int cung2l_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cung2r_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cungbr_(char *vect, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work,
    integer *lwork, integer *info);

int cunghr_(integer *n, integer *ilo, integer *ihi, complex *a, integer *lda, complex *tau, complex *work,
    integer *lwork, integer *info);

int cungl2_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cunglq_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *lwork,
    integer *info);

int cungql_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *lwork,
    integer *info);

int cungqr_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *lwork,
    integer *info);

int cungr2_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *info);

int cungrq_(integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau, complex *work, integer *lwork,
    integer *info);

int cungtr_(
    char *uplo, integer *n, complex *a, integer *lda, complex *tau, complex *work, integer *lwork, integer *info);

int cunm2l_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *info);

int cunm2r_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *info);

int cunmbr_(char *vect, char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda,
    complex *tau, complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cunmhr_(char *side, char *trans, integer *m, integer *n, integer *ilo, integer *ihi, complex *a, integer *lda,
    complex *tau, complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cunml2_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *info);

int cunmlq_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cunmql_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cunmqr_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cunmr2_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *info);

int cunmr3_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, complex *a, integer *lda,
    complex *tau, complex *c__, integer *ldc, complex *work, integer *info);

int cunmrq_(char *side, char *trans, integer *m, integer *n, integer *k, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cunmrz_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, complex *a, integer *lda,
    complex *tau, complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cunmtr_(char *side, char *uplo, char *trans, integer *m, integer *n, complex *a, integer *lda, complex *tau,
    complex *c__, integer *ldc, complex *work, integer *lwork, integer *info);

int cupgtr_(char *uplo, integer *n, complex *ap, complex *tau, complex *q, integer *ldq, complex *work, integer *info);

int cupmtr_(char *side, char *uplo, char *trans, integer *m, integer *n, complex *ap, complex *tau, complex *c__,
    integer *ldc, complex *work, integer *info);

int dbdsdc_(char *uplo, char *compq, integer *n, doublereal *d__, doublereal *e, doublereal *u, integer *ldu,
    doublereal *vt, integer *ldvt, doublereal *q, integer *iq, doublereal *work, integer *iwork, integer *info);

int dbdsqr_(char *uplo, integer *n, integer *ncvt, integer *nru, integer *ncc, doublereal *d__, doublereal *e,
    doublereal *vt, integer *ldvt, doublereal *u, integer *ldu, doublereal *c__, integer *ldc, doublereal *work,
    integer *info);

int ddisna_(char *job, integer *m, integer *n, doublereal *d__, doublereal *sep, integer *info);

int dgbbrd_(char *vect, integer *m, integer *n, integer *ncc, integer *kl, integer *ku, doublereal *ab, integer *ldab,
    doublereal *d__, doublereal *e, doublereal *q, integer *ldq, doublereal *pt, integer *ldpt, doublereal *c__,
    integer *ldc, doublereal *work, integer *info);

int dgbcon_(char *norm, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab, integer *ipiv,
    doublereal *anorm, doublereal *rcond, doublereal *work, integer *iwork, integer *info);

int dgbequ_(integer *m, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab, doublereal *r__,
    doublereal *c__, doublereal *rowcnd, doublereal *colcnd, doublereal *amax, integer *info);

int dgbequb_(integer *m, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab, doublereal *r__,
    doublereal *c__, doublereal *rowcnd, doublereal *colcnd, doublereal *amax, integer *info);

int dgbrfs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublereal *ab, integer *ldab,
    doublereal *afb, integer *ldafb, integer *ipiv, doublereal *b, integer *ldb, doublereal *x, integer *ldx,
    doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork, integer *info);

int dgbrfsx_(char *trans, char *equed, integer *n, integer *kl, integer *ku, integer *nrhs, doublereal *ab,
    integer *ldab, doublereal *afb, integer *ldafb, integer *ipiv, doublereal *r__, doublereal *c__, doublereal *b,
    integer *ldb, doublereal *x, integer *ldx, doublereal *rcond, doublereal *berr, integer *n_err_bnds__,
    doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublereal *work,
    integer *iwork, integer *info);

int dgbsv_(integer *n, integer *kl, integer *ku, integer *nrhs, doublereal *ab, integer *ldab, integer *ipiv,
    doublereal *b, integer *ldb, integer *info);

int dgbsvx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublereal *ab, integer *ldab,
    doublereal *afb, integer *ldafb, integer *ipiv, char *equed, doublereal *r__, doublereal *c__, doublereal *b,
    integer *ldb, doublereal *x, integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr, doublereal *work,
    integer *iwork, integer *info);

int dgbsvxx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublereal *ab,
    integer *ldab, doublereal *afb, integer *ldafb, integer *ipiv, char *equed, doublereal *r__, doublereal *c__,
    doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *rcond, doublereal *rpvgrw, doublereal *berr,
    integer *n_err_bnds__, doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams,
    doublereal *params, doublereal *work, integer *iwork, integer *info);

int dgbtf2_(
    integer *m, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab, integer *ipiv, integer *info);

int dgbtrf_(
    integer *m, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab, integer *ipiv, integer *info);

int dgbtrs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublereal *ab, integer *ldab,
    integer *ipiv, doublereal *b, integer *ldb, integer *info);

int dgebak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, doublereal *scale, integer *m, doublereal *v,
    integer *ldv, integer *info);

int dgebal_(
    char *job, integer *n, doublereal *a, integer *lda, integer *ilo, integer *ihi, doublereal *scale, integer *info);

int dgebd2_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *d__, doublereal *e, doublereal *tauq,
    doublereal *taup, doublereal *work, integer *info);

int dgebrd_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *d__, doublereal *e, doublereal *tauq,
    doublereal *taup, doublereal *work, integer *lwork, integer *info);

int dgecon_(char *norm, integer *n, doublereal *a, integer *lda, doublereal *anorm, doublereal *rcond, doublereal *work,
    integer *iwork, integer *info);

int dgeequ_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *r__, doublereal *c__, doublereal *rowcnd,
    doublereal *colcnd, doublereal *amax, integer *info);

int dgeequb_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *r__, doublereal *c__, doublereal *rowcnd,
    doublereal *colcnd, doublereal *amax, integer *info);

int dgees_(char *jobvs, char *sort, L_fp select, integer *n, doublereal *a, integer *lda, integer *sdim, doublereal *wr,
    doublereal *wi, doublereal *vs, integer *ldvs, doublereal *work, integer *lwork, logical *bwork, integer *info);

int dgeesx_(char *jobvs, char *sort, L_fp select, char *sense, integer *n, doublereal *a, integer *lda, integer *sdim,
    doublereal *wr, doublereal *wi, doublereal *vs, integer *ldvs, doublereal *rconde, doublereal *rcondv,
    doublereal *work, integer *lwork, integer *iwork, integer *liwork, logical *bwork, integer *info);

int dgeev_(char *jobvl, char *jobvr, integer *n, doublereal *a, integer *lda, doublereal *wr, doublereal *wi,
    doublereal *vl, integer *ldvl, doublereal *vr, integer *ldvr, doublereal *work, integer *lwork, integer *info);

int dgeevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, doublereal *a, integer *lda,
    doublereal *wr, doublereal *wi, doublereal *vl, integer *ldvl, doublereal *vr, integer *ldvr, integer *ilo,
    integer *ihi, doublereal *scale, doublereal *abnrm, doublereal *rconde, doublereal *rcondv, doublereal *work,
    integer *lwork, integer *iwork, integer *info);

int dgegs_(char *jobvsl, char *jobvsr, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *alphar, doublereal *alphai, doublereal *beta, doublereal *vsl, integer *ldvsl, doublereal *vsr,
    integer *ldvsr, doublereal *work, integer *lwork, integer *info);

int dgegv_(char *jobvl, char *jobvr, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *alphar, doublereal *alphai, doublereal *beta, doublereal *vl, integer *ldvl, doublereal *vr,
    integer *ldvr, doublereal *work, integer *lwork, integer *info);

int dgehd2_(integer *n, integer *ilo, integer *ihi, doublereal *a, integer *lda, doublereal *tau, doublereal *work,
    integer *info);

int dgehrd_(integer *n, integer *ilo, integer *ihi, doublereal *a, integer *lda, doublereal *tau, doublereal *work,
    integer *lwork, integer *info);

int dgejsv_(char *joba, char *jobu, char *jobv, char *jobr, char *jobt, char *jobp, integer *m, integer *n,
    doublereal *a, integer *lda, doublereal *sva, doublereal *u, integer *ldu, doublereal *v, integer *ldv,
    doublereal *work, integer *lwork, integer *iwork, integer *info);

int dgelq2_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dgelqf_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
    integer *info);

int dgels_(char *trans, integer *m, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *work, integer *lwork, integer *info);

int dgelsd_(integer *m, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *s, doublereal *rcond, integer *rank, doublereal *work, integer *lwork, integer *iwork, integer *info);

int dgelss_(integer *m, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *s, doublereal *rcond, integer *rank, doublereal *work, integer *lwork, integer *info);

int dgelsx_(integer *m, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    integer *jpvt, doublereal *rcond, integer *rank, doublereal *work, integer *info);

int dgelsy_(integer *m, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    integer *jpvt, doublereal *rcond, integer *rank, doublereal *work, integer *lwork, integer *info);

int dgeql2_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dgeqlf_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
    integer *info);

int dgeqp3_(integer *m, integer *n, doublereal *a, integer *lda, integer *jpvt, doublereal *tau, doublereal *work,
    integer *lwork, integer *info);

int dgeqpf_(integer *m, integer *n, doublereal *a, integer *lda, integer *jpvt, doublereal *tau, doublereal *work,
    integer *info);

int dgeqr2_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dgeqrf_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
    integer *info);

int dgerfs_(char *trans, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af, integer *ldaf,
    integer *ipiv, doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublereal *work, integer *iwork, integer *info);

int dgerfsx_(char *trans, char *equed, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, integer *ipiv, doublereal *r__, doublereal *c__, doublereal *b, integer *ldb, doublereal *x,
    integer *ldx, doublereal *rcond, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublereal *work, integer *iwork, integer *info);

int dgerq2_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dgerqf_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
    integer *info);

int dgesc2_(integer *n, doublereal *a, integer *lda, doublereal *rhs, integer *ipiv, integer *jpiv, doublereal *scale);

int dgesdd_(char *jobz, integer *m, integer *n, doublereal *a, integer *lda, doublereal *s, doublereal *u, integer *ldu,
    doublereal *vt, integer *ldvt, doublereal *work, integer *lwork, integer *iwork, integer *info);

int dgesv_(
    integer *n, integer *nrhs, doublereal *a, integer *lda, integer *ipiv, doublereal *b, integer *ldb, integer *info);

int dgesvd_(char *jobu, char *jobvt, integer *m, integer *n, doublereal *a, integer *lda, doublereal *s, doublereal *u,
    integer *ldu, doublereal *vt, integer *ldvt, doublereal *work, integer *lwork, integer *info);

int dgesvj_(char *joba, char *jobu, char *jobv, integer *m, integer *n, doublereal *a, integer *lda, doublereal *sva,
    integer *mv, doublereal *v, integer *ldv, doublereal *work, integer *lwork, integer *info);

int dgesvx_(char *fact, char *trans, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, integer *ipiv, char *equed, doublereal *r__, doublereal *c__, doublereal *b, integer *ldb,
    doublereal *x, integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr, doublereal *work,
    integer *iwork, integer *info);

int dgesvxx_(char *fact, char *trans, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, integer *ipiv, char *equed, doublereal *r__, doublereal *c__, doublereal *b, integer *ldb,
    doublereal *x, integer *ldx, doublereal *rcond, doublereal *rpvgrw, doublereal *berr, integer *n_err_bnds__,
    doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublereal *work,
    integer *iwork, integer *info);

int dgetc2_(integer *n, doublereal *a, integer *lda, integer *ipiv, integer *jpiv, integer *info);

int dgetf2_(integer *m, integer *n, doublereal *a, integer *lda, integer *ipiv, integer *info);

int dgetrf_(integer *m, integer *n, doublereal *a, integer *lda, integer *ipiv, integer *info);

int dgetri_(integer *n, doublereal *a, integer *lda, integer *ipiv, doublereal *work, integer *lwork, integer *info);

int dgetrs_(char *trans, integer *n, integer *nrhs, doublereal *a, integer *lda, integer *ipiv, doublereal *b,
    integer *ldb, integer *info);

int dggbak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, doublereal *lscale, doublereal *rscale,
    integer *m, doublereal *v, integer *ldv, integer *info);

int dggbal_(char *job, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb, integer *ilo, integer *ihi,
    doublereal *lscale, doublereal *rscale, doublereal *work, integer *info);

int dgges_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, integer *sdim, doublereal *alphar, doublereal *alphai, doublereal *beta, doublereal *vsl,
    integer *ldvsl, doublereal *vsr, integer *ldvsr, doublereal *work, integer *lwork, logical *bwork, integer *info);

int dggesx_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, char *sense, integer *n, doublereal *a, integer *lda,
    doublereal *b, integer *ldb, integer *sdim, doublereal *alphar, doublereal *alphai, doublereal *beta,
    doublereal *vsl, integer *ldvsl, doublereal *vsr, integer *ldvsr, doublereal *rconde, doublereal *rcondv,
    doublereal *work, integer *lwork, integer *iwork, integer *liwork, logical *bwork, integer *info);

int dggev_(char *jobvl, char *jobvr, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *alphar, doublereal *alphai, doublereal *beta, doublereal *vl, integer *ldvl, doublereal *vr,
    integer *ldvr, doublereal *work, integer *lwork, integer *info);

int dggevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *alphar, doublereal *alphai, doublereal *beta, doublereal *vl, integer *ldvl,
    doublereal *vr, integer *ldvr, integer *ilo, integer *ihi, doublereal *lscale, doublereal *rscale,
    doublereal *abnrm, doublereal *bbnrm, doublereal *rconde, doublereal *rcondv, doublereal *work, integer *lwork,
    integer *iwork, logical *bwork, integer *info);

int dggglm_(integer *n, integer *m, integer *p, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *d__, doublereal *x, doublereal *y, doublereal *work, integer *lwork, integer *info);

int dgghrd_(char *compq, char *compz, integer *n, integer *ilo, integer *ihi, doublereal *a, integer *lda,
    doublereal *b, integer *ldb, doublereal *q, integer *ldq, doublereal *z__, integer *ldz, integer *info);

int dgglse_(integer *m, integer *n, integer *p, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *c__, doublereal *d__, doublereal *x, doublereal *work, integer *lwork, integer *info);

int dggqrf_(integer *n, integer *m, integer *p, doublereal *a, integer *lda, doublereal *taua, doublereal *b,
    integer *ldb, doublereal *taub, doublereal *work, integer *lwork, integer *info);

int dggrqf_(integer *m, integer *p, integer *n, doublereal *a, integer *lda, doublereal *taua, doublereal *b,
    integer *ldb, doublereal *taub, doublereal *work, integer *lwork, integer *info);

int dggsvd_(char *jobu, char *jobv, char *jobq, integer *m, integer *n, integer *p, integer *k, integer *l,
    doublereal *a, integer *lda, doublereal *b, integer *ldb, doublereal *alpha, doublereal *beta, doublereal *u,
    integer *ldu, doublereal *v, integer *ldv, doublereal *q, integer *ldq, doublereal *work, integer *iwork,
    integer *info);

int dggsvp_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, doublereal *a, integer *lda,
    doublereal *b, integer *ldb, doublereal *tola, doublereal *tolb, integer *k, integer *l, doublereal *u,
    integer *ldu, doublereal *v, integer *ldv, doublereal *q, integer *ldq, integer *iwork, doublereal *tau,
    doublereal *work, integer *info);

int dgsvj0_(char *jobv, integer *m, integer *n, doublereal *a, integer *lda, doublereal *d__, doublereal *sva,
    integer *mv, doublereal *v, integer *ldv, doublereal *eps, doublereal *sfmin, doublereal *tol, integer *nsweep,
    doublereal *work, integer *lwork, integer *info);

int dgsvj1_(char *jobv, integer *m, integer *n, integer *n1, doublereal *a, integer *lda, doublereal *d__,
    doublereal *sva, integer *mv, doublereal *v, integer *ldv, doublereal *eps, doublereal *sfmin, doublereal *tol,
    integer *nsweep, doublereal *work, integer *lwork, integer *info);

int dgtcon_(char *norm, integer *n, doublereal *dl, doublereal *d__, doublereal *du, doublereal *du2, integer *ipiv,
    doublereal *anorm, doublereal *rcond, doublereal *work, integer *iwork, integer *info);

int dgtrfs_(char *trans, integer *n, integer *nrhs, doublereal *dl, doublereal *d__, doublereal *du, doublereal *dlf,
    doublereal *df, doublereal *duf, doublereal *du2, integer *ipiv, doublereal *b, integer *ldb, doublereal *x,
    integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork, integer *info);

int dgtsv_(integer *n, integer *nrhs, doublereal *dl, doublereal *d__, doublereal *du, doublereal *b, integer *ldb,
    integer *info);

int dgtsvx_(char *fact, char *trans, integer *n, integer *nrhs, doublereal *dl, doublereal *d__, doublereal *du,
    doublereal *dlf, doublereal *df, doublereal *duf, doublereal *du2, integer *ipiv, doublereal *b, integer *ldb,
    doublereal *x, integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr, doublereal *work,
    integer *iwork, integer *info);

int dgttrf_(integer *n, doublereal *dl, doublereal *d__, doublereal *du, doublereal *du2, integer *ipiv, integer *info);

int dgttrs_(char *trans, integer *n, integer *nrhs, doublereal *dl, doublereal *d__, doublereal *du, doublereal *du2,
    integer *ipiv, doublereal *b, integer *ldb, integer *info);

int dgtts2_(integer *itrans, integer *n, integer *nrhs, doublereal *dl, doublereal *d__, doublereal *du,
    doublereal *du2, integer *ipiv, doublereal *b, integer *ldb);

int dhgeqz_(char *job, char *compq, char *compz, integer *n, integer *ilo, integer *ihi, doublereal *h__, integer *ldh,
    doublereal *t, integer *ldt, doublereal *alphar, doublereal *alphai, doublereal *beta, doublereal *q, integer *ldq,
    doublereal *z__, integer *ldz, doublereal *work, integer *lwork, integer *info);

int dhsein_(char *side, char *eigsrc, char *initv, logical *select, integer *n, doublereal *h__, integer *ldh,
    doublereal *wr, doublereal *wi, doublereal *vl, integer *ldvl, doublereal *vr, integer *ldvr, integer *mm,
    integer *m, doublereal *work, integer *ifaill, integer *ifailr, integer *info);

int dhseqr_(char *job, char *compz, integer *n, integer *ilo, integer *ihi, doublereal *h__, integer *ldh,
    doublereal *wr, doublereal *wi, doublereal *z__, integer *ldz, doublereal *work, integer *lwork, integer *info);

logical disnan_(doublereal *din);

int dla_gbamv__(integer *trans, integer *m, integer *n, integer *kl, integer *ku, doublereal *alpha, doublereal *ab,
    integer *ldab, doublereal *x, integer *incx, doublereal *beta, doublereal *y, integer *incy);

doublereal dla_gbrcond__(char *trans, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab,
    doublereal *afb, integer *ldafb, integer *ipiv, integer *cmode, doublereal *c__, integer *info, doublereal *work,
    integer *iwork, ftnlen trans_len);

int dla_gbrfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *kl, integer *ku,
    integer *nrhs, doublereal *ab, integer *ldab, doublereal *afb, integer *ldafb, integer *ipiv, logical *colequ,
    doublereal *c__, doublereal *b, integer *ldb, doublereal *y, integer *ldy, doublereal *berr_out__,
    integer *n_norms__, doublereal *errs_n__, doublereal *errs_c__, doublereal *res, doublereal *ayb, doublereal *dy,
    doublereal *y_tail__, doublereal *rcond, integer *ithresh, doublereal *rthresh, doublereal *dz_ub__,
    logical *ignore_cwise__, integer *info);

doublereal dla_gbrpvgrw__(integer *n, integer *kl, integer *ku, integer *ncols, doublereal *ab, integer *ldab,
    doublereal *afb, integer *ldafb);

int dla_geamv__(integer *trans, integer *m, integer *n, doublereal *alpha, doublereal *a, integer *lda, doublereal *x,
    integer *incx, doublereal *beta, doublereal *y, integer *incy);

doublereal dla_gercond__(char *trans, integer *n, doublereal *a, integer *lda, doublereal *af, integer *ldaf,
    integer *ipiv, integer *cmode, doublereal *c__, integer *info, doublereal *work, integer *iwork, ftnlen trans_len);

int dla_gerfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *nrhs, doublereal *a,
    integer *lda, doublereal *af, integer *ldaf, integer *ipiv, logical *colequ, doublereal *c__, doublereal *b,
    integer *ldb, doublereal *y, integer *ldy, doublereal *berr_out__, integer *n_norms__, doublereal *errs_n__,
    doublereal *errs_c__, doublereal *res, doublereal *ayb, doublereal *dy, doublereal *y_tail__, doublereal *rcond,
    integer *ithresh, doublereal *rthresh, doublereal *dz_ub__, logical *ignore_cwise__, integer *info);

int dla_lin_berr__(integer *n, integer *nz, integer *nrhs, doublereal *res, doublereal *ayb, doublereal *berr);

doublereal dla_porcond__(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *af, integer *ldaf,
    integer *cmode, doublereal *c__, integer *info, doublereal *work, integer *iwork, ftnlen uplo_len);

int dla_porfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda,
    doublereal *af, integer *ldaf, logical *colequ, doublereal *c__, doublereal *b, integer *ldb, doublereal *y,
    integer *ldy, doublereal *berr_out__, integer *n_norms__, doublereal *errs_n__, doublereal *errs_c__,
    doublereal *res, doublereal *ayb, doublereal *dy, doublereal *y_tail__, doublereal *rcond, integer *ithresh,
    doublereal *rthresh, doublereal *dz_ub__, logical *ignore_cwise__, integer *info, ftnlen uplo_len);

doublereal dla_porpvgrw__(char *uplo, integer *ncols, doublereal *a, integer *lda, doublereal *af, integer *ldaf,
    doublereal *work, ftnlen uplo_len);

doublereal dla_rpvgrw__(integer *n, integer *ncols, doublereal *a, integer *lda, doublereal *af, integer *ldaf);

int dla_syamv__(integer *uplo, integer *n, doublereal *alpha, doublereal *a, integer *lda, doublereal *x, integer *incx,
    doublereal *beta, doublereal *y, integer *incy);

doublereal dla_syrcond__(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *af, integer *ldaf,
    integer *ipiv, integer *cmode, doublereal *c__, integer *info, doublereal *work, integer *iwork, ftnlen uplo_len);

int dla_syrfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda,
    doublereal *af, integer *ldaf, integer *ipiv, logical *colequ, doublereal *c__, doublereal *b, integer *ldb,
    doublereal *y, integer *ldy, doublereal *berr_out__, integer *n_norms__, doublereal *errs_n__, doublereal *errs_c__,
    doublereal *res, doublereal *ayb, doublereal *dy, doublereal *y_tail__, doublereal *rcond, integer *ithresh,
    doublereal *rthresh, doublereal *dz_ub__, logical *ignore_cwise__, integer *info, ftnlen uplo_len);

doublereal dla_syrpvgrw__(char *uplo, integer *n, integer *info, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, integer *ipiv, doublereal *work, ftnlen uplo_len);

int dla_wwaddw__(integer *n, doublereal *x, doublereal *y, doublereal *w);

int dlabad_(doublereal *small, doublereal *large);

int dlabrd_(integer *m, integer *n, integer *nb, doublereal *a, integer *lda, doublereal *d__, doublereal *e,
    doublereal *tauq, doublereal *taup, doublereal *x, integer *ldx, doublereal *y, integer *ldy);

int dlacn2_(integer *n, doublereal *v, doublereal *x, integer *isgn, doublereal *est, integer *kase, integer *isave);

int dlacon_(integer *n, doublereal *v, doublereal *x, integer *isgn, doublereal *est, integer *kase);

int dlacpy_(char *uplo, integer *m, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb);

int dladiv_(doublereal *a, doublereal *b, doublereal *c__, doublereal *d__, doublereal *p, doublereal *q);

int dlae2_(doublereal *a, doublereal *b, doublereal *c__, doublereal *rt1, doublereal *rt2);

int dlaebz_(integer *ijob, integer *nitmax, integer *n, integer *mmax, integer *minp, integer *nbmin,
    doublereal *abstol, doublereal *reltol, doublereal *pivmin, doublereal *d__, doublereal *e, doublereal *e2,
    integer *nval, doublereal *ab, doublereal *c__, integer *mout, integer *nab, doublereal *work, integer *iwork,
    integer *info);

int dlaed0_(integer *icompq, integer *qsiz, integer *n, doublereal *d__, doublereal *e, doublereal *q, integer *ldq,
    doublereal *qstore, integer *ldqs, doublereal *work, integer *iwork, integer *info);

int dlaed1_(integer *n, doublereal *d__, doublereal *q, integer *ldq, integer *indxq, doublereal *rho, integer *cutpnt,
    doublereal *work, integer *iwork, integer *info);

int dlaed2_(integer *k, integer *n, integer *n1, doublereal *d__, doublereal *q, integer *ldq, integer *indxq,
    doublereal *rho, doublereal *z__, doublereal *dlamda, doublereal *w, doublereal *q2, integer *indx, integer *indxc,
    integer *indxp, integer *coltyp, integer *info);

int dlaed3_(integer *k, integer *n, integer *n1, doublereal *d__, doublereal *q, integer *ldq, doublereal *rho,
    doublereal *dlamda, doublereal *q2, integer *indx, integer *ctot, doublereal *w, doublereal *s, integer *info);

int dlaed4_(integer *n, integer *i__, doublereal *d__, doublereal *z__, doublereal *delta, doublereal *rho,
    doublereal *dlam, integer *info);

int dlaed5_(integer *i__, doublereal *d__, doublereal *z__, doublereal *delta, doublereal *rho, doublereal *dlam);

int dlaed6_(integer *kniter, logical *orgati, doublereal *rho, doublereal *d__, doublereal *z__, doublereal *finit,
    doublereal *tau, integer *info);

int dlaed7_(integer *icompq, integer *n, integer *qsiz, integer *tlvls, integer *curlvl, integer *curpbm,
    doublereal *d__, doublereal *q, integer *ldq, integer *indxq, doublereal *rho, integer *cutpnt, doublereal *qstore,
    integer *qptr, integer *prmptr, integer *perm, integer *givptr, integer *givcol, doublereal *givnum,
    doublereal *work, integer *iwork, integer *info);

int dlaed8_(integer *icompq, integer *k, integer *n, integer *qsiz, doublereal *d__, doublereal *q, integer *ldq,
    integer *indxq, doublereal *rho, integer *cutpnt, doublereal *z__, doublereal *dlamda, doublereal *q2,
    integer *ldq2, doublereal *w, integer *perm, integer *givptr, integer *givcol, doublereal *givnum, integer *indxp,
    integer *indx, integer *info);

int dlaed9_(integer *k, integer *kstart, integer *kstop, integer *n, doublereal *d__, doublereal *q, integer *ldq,
    doublereal *rho, doublereal *dlamda, doublereal *w, doublereal *s, integer *lds, integer *info);

int dlaeda_(integer *n, integer *tlvls, integer *curlvl, integer *curpbm, integer *prmptr, integer *perm,
    integer *givptr, integer *givcol, doublereal *givnum, doublereal *q, integer *qptr, doublereal *z__,
    doublereal *ztemp, integer *info);

int dlaein_(logical *rightv, logical *noinit, integer *n, doublereal *h__, integer *ldh, doublereal *wr, doublereal *wi,
    doublereal *vr, doublereal *vi, doublereal *b, integer *ldb, doublereal *work, doublereal *eps3, doublereal *smlnum,
    doublereal *bignum, integer *info);

int dlaev2_(
    doublereal *a, doublereal *b, doublereal *c__, doublereal *rt1, doublereal *rt2, doublereal *cs1, doublereal *sn1);

int dlaexc_(logical *wantq, integer *n, doublereal *t, integer *ldt, doublereal *q, integer *ldq, integer *j1,
    integer *n1, integer *n2, doublereal *work, integer *info);

int dlag2_(doublereal *a, integer *lda, doublereal *b, integer *ldb, doublereal *safmin, doublereal *scale1,
    doublereal *scale2, doublereal *wr1, doublereal *wr2, doublereal *wi);

int dlag2s_(integer *m, integer *n, doublereal *a, integer *lda, real *sa, integer *ldsa, integer *info);

int dlags2_(logical *upper, doublereal *a1, doublereal *a2, doublereal *a3, doublereal *b1, doublereal *b2,
    doublereal *b3, doublereal *csu, doublereal *snu, doublereal *csv, doublereal *snv, doublereal *csq,
    doublereal *snq);

int dlagtf_(integer *n, doublereal *a, doublereal *lambda, doublereal *b, doublereal *c__, doublereal *tol,
    doublereal *d__, integer *in, integer *info);

int dlagtm_(char *trans, integer *n, integer *nrhs, doublereal *alpha, doublereal *dl, doublereal *d__, doublereal *du,
    doublereal *x, integer *ldx, doublereal *beta, doublereal *b, integer *ldb);

int dlagts_(integer *job, integer *n, doublereal *a, doublereal *b, doublereal *c__, doublereal *d__, integer *in,
    doublereal *y, doublereal *tol, integer *info);

int dlagv2_(doublereal *a, integer *lda, doublereal *b, integer *ldb, doublereal *alphar, doublereal *alphai,
    doublereal *beta, doublereal *csl, doublereal *snl, doublereal *csr, doublereal *snr);

int dlahqr_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, doublereal *h__, integer *ldh,
    doublereal *wr, doublereal *wi, integer *iloz, integer *ihiz, doublereal *z__, integer *ldz, integer *info);

int dlahr2_(integer *n, integer *k, integer *nb, doublereal *a, integer *lda, doublereal *tau, doublereal *t,
    integer *ldt, doublereal *y, integer *ldy);

int dlahrd_(integer *n, integer *k, integer *nb, doublereal *a, integer *lda, doublereal *tau, doublereal *t,
    integer *ldt, doublereal *y, integer *ldy);

int dlaic1_(integer *job, integer *j, doublereal *x, doublereal *sest, doublereal *w, doublereal *gamma,
    doublereal *sestpr, doublereal *s, doublereal *c__);

logical dlaisnan_(doublereal *din1, doublereal *din2);

int dlaln2_(logical *ltrans, integer *na, integer *nw, doublereal *smin, doublereal *ca, doublereal *a, integer *lda,
    doublereal *d1, doublereal *d2, doublereal *b, integer *ldb, doublereal *wr, doublereal *wi, doublereal *x,
    integer *ldx, doublereal *scale, doublereal *xnorm, integer *info);

int dlals0_(integer *icompq, integer *nl, integer *nr, integer *sqre, integer *nrhs, doublereal *b, integer *ldb,
    doublereal *bx, integer *ldbx, integer *perm, integer *givptr, integer *givcol, integer *ldgcol, doublereal *givnum,
    integer *ldgnum, doublereal *poles, doublereal *difl, doublereal *difr, doublereal *z__, integer *k,
    doublereal *c__, doublereal *s, doublereal *work, integer *info);

int dlalsa_(integer *icompq, integer *smlsiz, integer *n, integer *nrhs, doublereal *b, integer *ldb, doublereal *bx,
    integer *ldbx, doublereal *u, integer *ldu, doublereal *vt, integer *k, doublereal *difl, doublereal *difr,
    doublereal *z__, doublereal *poles, integer *givptr, integer *givcol, integer *ldgcol, integer *perm,
    doublereal *givnum, doublereal *c__, doublereal *s, doublereal *work, integer *iwork, integer *info);

int dlalsd_(char *uplo, integer *smlsiz, integer *n, integer *nrhs, doublereal *d__, doublereal *e, doublereal *b,
    integer *ldb, doublereal *rcond, integer *rank, doublereal *work, integer *iwork, integer *info);

int dlamrg_(integer *n1, integer *n2, doublereal *a, integer *dtrd1, integer *dtrd2, integer *index);

integer dlaneg_(integer *n, doublereal *d__, doublereal *lld, doublereal *sigma, doublereal *pivmin, integer *r__);

doublereal dlangb_(char *norm, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab, doublereal *work);

doublereal dlange_(char *norm, integer *m, integer *n, doublereal *a, integer *lda, doublereal *work);

doublereal dlangt_(char *norm, integer *n, doublereal *dl, doublereal *d__, doublereal *du);

doublereal dlanhs_(char *norm, integer *n, doublereal *a, integer *lda, doublereal *work);

doublereal dlansb_(char *norm, char *uplo, integer *n, integer *k, doublereal *ab, integer *ldab, doublereal *work);

doublereal dlansf_(char *norm, char *transr, char *uplo, integer *n, doublereal *a, doublereal *work);

doublereal dlansp_(char *norm, char *uplo, integer *n, doublereal *ap, doublereal *work);

doublereal dlanst_(char *norm, integer *n, doublereal *d__, doublereal *e);

doublereal dlansy_(char *norm, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *work);

doublereal dlantb_(
    char *norm, char *uplo, char *diag, integer *n, integer *k, doublereal *ab, integer *ldab, doublereal *work);

doublereal dlantp_(char *norm, char *uplo, char *diag, integer *n, doublereal *ap, doublereal *work);

doublereal dlantr_(
    char *norm, char *uplo, char *diag, integer *m, integer *n, doublereal *a, integer *lda, doublereal *work);

int dlanv2_(doublereal *a, doublereal *b, doublereal *c__, doublereal *d__, doublereal *rt1r, doublereal *rt1i,
    doublereal *rt2r, doublereal *rt2i, doublereal *cs, doublereal *sn);

int dlapll_(integer *n, doublereal *x, integer *incx, doublereal *y, integer *incy, doublereal *ssmin);

int dlapmt_(logical *forwrd, integer *m, integer *n, doublereal *x, integer *ldx, integer *k);

doublereal dlapy2_(doublereal *x, doublereal *y);

doublereal dlapy3_(doublereal *x, doublereal *y, doublereal *z__);

int dlaqgb_(integer *m, integer *n, integer *kl, integer *ku, doublereal *ab, integer *ldab, doublereal *r__,
    doublereal *c__, doublereal *rowcnd, doublereal *colcnd, doublereal *amax, char *equed);

int dlaqge_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *r__, doublereal *c__, doublereal *rowcnd,
    doublereal *colcnd, doublereal *amax, char *equed);

int dlaqp2_(integer *m, integer *n, integer *offset, doublereal *a, integer *lda, integer *jpvt, doublereal *tau,
    doublereal *vn1, doublereal *vn2, doublereal *work);

int dlaqps_(integer *m, integer *n, integer *offset, integer *nb, integer *kb, doublereal *a, integer *lda,
    integer *jpvt, doublereal *tau, doublereal *vn1, doublereal *vn2, doublereal *auxv, doublereal *f, integer *ldf);

int dlaqr0_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, doublereal *h__, integer *ldh,
    doublereal *wr, doublereal *wi, integer *iloz, integer *ihiz, doublereal *z__, integer *ldz, doublereal *work,
    integer *lwork, integer *info);

int dlaqr1_(integer *n, doublereal *h__, integer *ldh, doublereal *sr1, doublereal *si1, doublereal *sr2,
    doublereal *si2, doublereal *v);

int dlaqr2_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, doublereal *h__,
    integer *ldh, integer *iloz, integer *ihiz, doublereal *z__, integer *ldz, integer *ns, integer *nd, doublereal *sr,
    doublereal *si, doublereal *v, integer *ldv, integer *nh, doublereal *t, integer *ldt, integer *nv, doublereal *wv,
    integer *ldwv, doublereal *work, integer *lwork);

int dlaqr3_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, doublereal *h__,
    integer *ldh, integer *iloz, integer *ihiz, doublereal *z__, integer *ldz, integer *ns, integer *nd, doublereal *sr,
    doublereal *si, doublereal *v, integer *ldv, integer *nh, doublereal *t, integer *ldt, integer *nv, doublereal *wv,
    integer *ldwv, doublereal *work, integer *lwork);

int dlaqr4_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, doublereal *h__, integer *ldh,
    doublereal *wr, doublereal *wi, integer *iloz, integer *ihiz, doublereal *z__, integer *ldz, doublereal *work,
    integer *lwork, integer *info);

int dlaqr5_(logical *wantt, logical *wantz, integer *kacc22, integer *n, integer *ktop, integer *kbot, integer *nshfts,
    doublereal *sr, doublereal *si, doublereal *h__, integer *ldh, integer *iloz, integer *ihiz, doublereal *z__,
    integer *ldz, doublereal *v, integer *ldv, doublereal *u, integer *ldu, integer *nv, doublereal *wv, integer *ldwv,
    integer *nh, doublereal *wh, integer *ldwh);

int dlaqsb_(char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, doublereal *s, doublereal *scond,
    doublereal *amax, char *equed);

int dlaqsp_(char *uplo, integer *n, doublereal *ap, doublereal *s, doublereal *scond, doublereal *amax, char *equed);

int dlaqsy_(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax,
    char *equed);

int dlaqtr_(logical *ltran, logical *lreal, integer *n, doublereal *t, integer *ldt, doublereal *b, doublereal *w,
    doublereal *scale, doublereal *x, doublereal *work, integer *info);

int dlar1v_(integer *n, integer *b1, integer *bn, doublereal *lambda, doublereal *d__, doublereal *l, doublereal *ld,
    doublereal *lld, doublereal *pivmin, doublereal *gaptol, doublereal *z__, logical *wantnc, integer *negcnt,
    doublereal *ztz, doublereal *mingma, integer *r__, integer *isuppz, doublereal *nrminv, doublereal *resid,
    doublereal *rqcorr, doublereal *work);

int dlar2v_(integer *n, doublereal *x, doublereal *y, doublereal *z__, integer *incx, doublereal *c__, doublereal *s,
    integer *incc);

int dlarf_(char *side, integer *m, integer *n, doublereal *v, integer *incv, doublereal *tau, doublereal *c__,
    integer *ldc, doublereal *work);

int dlarfb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, doublereal *v,
    integer *ldv, doublereal *t, integer *ldt, doublereal *c__, integer *ldc, doublereal *work, integer *ldwork);

int dlarfg_(integer *n, doublereal *alpha, doublereal *x, integer *incx, doublereal *tau);

int dlarfp_(integer *n, doublereal *alpha, doublereal *x, integer *incx, doublereal *tau);

int dlarft_(char *direct, char *storev, integer *n, integer *k, doublereal *v, integer *ldv, doublereal *tau,
    doublereal *t, integer *ldt);

int dlarfx_(char *side, integer *m, integer *n, doublereal *v, doublereal *tau, doublereal *c__, integer *ldc,
    doublereal *work);

int dlargv_(integer *n, doublereal *x, integer *incx, doublereal *y, integer *incy, doublereal *c__, integer *incc);

int dlarnv_(integer *idist, integer *iseed, integer *n, doublereal *x);

int dlarra_(integer *n, doublereal *d__, doublereal *e, doublereal *e2, doublereal *spltol, doublereal *tnrm,
    integer *nsplit, integer *isplit, integer *info);

int dlarrb_(integer *n, doublereal *d__, doublereal *lld, integer *ifirst, integer *ilast, doublereal *rtol1,
    doublereal *rtol2, integer *offset, doublereal *w, doublereal *wgap, doublereal *werr, doublereal *work,
    integer *iwork, doublereal *pivmin, doublereal *spdiam, integer *twist, integer *info);

int dlarrc_(char *jobt, integer *n, doublereal *vl, doublereal *vu, doublereal *d__, doublereal *e, doublereal *pivmin,
    integer *eigcnt, integer *lcnt, integer *rcnt, integer *info);

int dlarrd_(char *range, char *order, integer *n, doublereal *vl, doublereal *vu, integer *il, integer *iu,
    doublereal *gers, doublereal *reltol, doublereal *d__, doublereal *e, doublereal *e2, doublereal *pivmin,
    integer *nsplit, integer *isplit, integer *m, doublereal *w, doublereal *werr, doublereal *wl, doublereal *wu,
    integer *iblock, integer *indexw, doublereal *work, integer *iwork, integer *info);

int dlarre_(char *range, integer *n, doublereal *vl, doublereal *vu, integer *il, integer *iu, doublereal *d__,
    doublereal *e, doublereal *e2, doublereal *rtol1, doublereal *rtol2, doublereal *spltol, integer *nsplit,
    integer *isplit, integer *m, doublereal *w, doublereal *werr, doublereal *wgap, integer *iblock, integer *indexw,
    doublereal *gers, doublereal *pivmin, doublereal *work, integer *iwork, integer *info);

int dlarrf_(integer *n, doublereal *d__, doublereal *l, doublereal *ld, integer *clstrt, integer *clend, doublereal *w,
    doublereal *wgap, doublereal *werr, doublereal *spdiam, doublereal *clgapl, doublereal *clgapr, doublereal *pivmin,
    doublereal *sigma, doublereal *dplus, doublereal *lplus, doublereal *work, integer *info);

int dlarrj_(integer *n, doublereal *d__, doublereal *e2, integer *ifirst, integer *ilast, doublereal *rtol,
    integer *offset, doublereal *w, doublereal *werr, doublereal *work, integer *iwork, doublereal *pivmin,
    doublereal *spdiam, integer *info);

int dlarrk_(integer *n, integer *iw, doublereal *gl, doublereal *gu, doublereal *d__, doublereal *e2,
    doublereal *pivmin, doublereal *reltol, doublereal *w, doublereal *werr, integer *info);

int dlarrr_(integer *n, doublereal *d__, doublereal *e, integer *info);

int dlarrv_(integer *n, doublereal *vl, doublereal *vu, doublereal *d__, doublereal *l, doublereal *pivmin,
    integer *isplit, integer *m, integer *dol, integer *dou, doublereal *minrgp, doublereal *rtol1, doublereal *rtol2,
    doublereal *w, doublereal *werr, doublereal *wgap, integer *iblock, integer *indexw, doublereal *gers,
    doublereal *z__, integer *ldz, integer *isuppz, doublereal *work, integer *iwork, integer *info);

int dlarscl2_(integer *m, integer *n, doublereal *d__, doublereal *x, integer *ldx);

int dlartg_(doublereal *f, doublereal *g, doublereal *cs, doublereal *sn, doublereal *r__);

int dlartv_(integer *n, doublereal *x, integer *incx, doublereal *y, integer *incy, doublereal *c__, doublereal *s,
    integer *incc);

int dlaruv_(integer *iseed, integer *n, doublereal *x);

int dlarz_(char *side, integer *m, integer *n, integer *l, doublereal *v, integer *incv, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work);

int dlarzb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, integer *l,
    doublereal *v, integer *ldv, doublereal *t, integer *ldt, doublereal *c__, integer *ldc, doublereal *work,
    integer *ldwork);

int dlarzt_(char *direct, char *storev, integer *n, integer *k, doublereal *v, integer *ldv, doublereal *tau,
    doublereal *t, integer *ldt);

int dlas2_(doublereal *f, doublereal *g, doublereal *h__, doublereal *ssmin, doublereal *ssmax);

int dlascl_(char *type__, integer *kl, integer *ku, doublereal *cfrom, doublereal *cto, integer *m, integer *n,
    doublereal *a, integer *lda, integer *info);

int dlascl2_(integer *m, integer *n, doublereal *d__, doublereal *x, integer *ldx);

int dlasd0_(integer *n, integer *sqre, doublereal *d__, doublereal *e, doublereal *u, integer *ldu, doublereal *vt,
    integer *ldvt, integer *smlsiz, integer *iwork, doublereal *work, integer *info);

int dlasd1_(integer *nl, integer *nr, integer *sqre, doublereal *d__, doublereal *alpha, doublereal *beta,
    doublereal *u, integer *ldu, doublereal *vt, integer *ldvt, integer *idxq, integer *iwork, doublereal *work,
    integer *info);

int dlasd2_(integer *nl, integer *nr, integer *sqre, integer *k, doublereal *d__, doublereal *z__, doublereal *alpha,
    doublereal *beta, doublereal *u, integer *ldu, doublereal *vt, integer *ldvt, doublereal *dsigma, doublereal *u2,
    integer *ldu2, doublereal *vt2, integer *ldvt2, integer *idxp, integer *idx, integer *idxc, integer *idxq,
    integer *coltyp, integer *info);

int dlasd3_(integer *nl, integer *nr, integer *sqre, integer *k, doublereal *d__, doublereal *q, integer *ldq,
    doublereal *dsigma, doublereal *u, integer *ldu, doublereal *u2, integer *ldu2, doublereal *vt, integer *ldvt,
    doublereal *vt2, integer *ldvt2, integer *idxc, integer *ctot, doublereal *z__, integer *info);

int dlasd4_(integer *n, integer *i__, doublereal *d__, doublereal *z__, doublereal *delta, doublereal *rho,
    doublereal *sigma, doublereal *work, integer *info);

int dlasd5_(integer *i__, doublereal *d__, doublereal *z__, doublereal *delta, doublereal *rho, doublereal *dsigma,
    doublereal *work);

int dlasd6_(integer *icompq, integer *nl, integer *nr, integer *sqre, doublereal *d__, doublereal *vf, doublereal *vl,
    doublereal *alpha, doublereal *beta, integer *idxq, integer *perm, integer *givptr, integer *givcol,
    integer *ldgcol, doublereal *givnum, integer *ldgnum, doublereal *poles, doublereal *difl, doublereal *difr,
    doublereal *z__, integer *k, doublereal *c__, doublereal *s, doublereal *work, integer *iwork, integer *info);

int dlasd7_(integer *icompq, integer *nl, integer *nr, integer *sqre, integer *k, doublereal *d__, doublereal *z__,
    doublereal *zw, doublereal *vf, doublereal *vfw, doublereal *vl, doublereal *vlw, doublereal *alpha,
    doublereal *beta, doublereal *dsigma, integer *idx, integer *idxp, integer *idxq, integer *perm, integer *givptr,
    integer *givcol, integer *ldgcol, doublereal *givnum, integer *ldgnum, doublereal *c__, doublereal *s,
    integer *info);

int dlasd8_(integer *icompq, integer *k, doublereal *d__, doublereal *z__, doublereal *vf, doublereal *vl,
    doublereal *difl, doublereal *difr, integer *lddifr, doublereal *dsigma, doublereal *work, integer *info);

int dlasda_(integer *icompq, integer *smlsiz, integer *n, integer *sqre, doublereal *d__, doublereal *e, doublereal *u,
    integer *ldu, doublereal *vt, integer *k, doublereal *difl, doublereal *difr, doublereal *z__, doublereal *poles,
    integer *givptr, integer *givcol, integer *ldgcol, integer *perm, doublereal *givnum, doublereal *c__,
    doublereal *s, doublereal *work, integer *iwork, integer *info);

int dlasdq_(char *uplo, integer *sqre, integer *n, integer *ncvt, integer *nru, integer *ncc, doublereal *d__,
    doublereal *e, doublereal *vt, integer *ldvt, doublereal *u, integer *ldu, doublereal *c__, integer *ldc,
    doublereal *work, integer *info);

int dlasdt_(integer *n, integer *lvl, integer *nd, integer *inode, integer *ndiml, integer *ndimr, integer *msub);

int dlaset_(char *uplo, integer *m, integer *n, doublereal *alpha, doublereal *beta, doublereal *a, integer *lda);

int dlasq1_(integer *n, doublereal *d__, doublereal *e, doublereal *work, integer *info);

int dlasq2_(integer *n, doublereal *z__, integer *info);

int dlasq3_(integer *i0, integer *n0, doublereal *z__, integer *pp, doublereal *dmin__, doublereal *sigma,
    doublereal *desig, doublereal *qmax, integer *nfail, integer *iter, integer *ndiv, logical *ieee, integer *ttype,
    doublereal *dmin1, doublereal *dmin2, doublereal *dn, doublereal *dn1, doublereal *dn2, doublereal *g,
    doublereal *tau);

int dlasq4_(integer *i0, integer *n0, doublereal *z__, integer *pp, integer *n0in, doublereal *dmin__,
    doublereal *dmin1, doublereal *dmin2, doublereal *dn, doublereal *dn1, doublereal *dn2, doublereal *tau,
    integer *ttype, doublereal *g);

int dlasq5_(integer *i0, integer *n0, doublereal *z__, integer *pp, doublereal *tau, doublereal *dmin__,
    doublereal *dmin1, doublereal *dmin2, doublereal *dn, doublereal *dnm1, doublereal *dnm2, logical *ieee);

int dlasq6_(integer *i0, integer *n0, doublereal *z__, integer *pp, doublereal *dmin__, doublereal *dmin1,
    doublereal *dmin2, doublereal *dn, doublereal *dnm1, doublereal *dnm2);

int dlasr_(char *side, char *pivot, char *direct, integer *m, integer *n, doublereal *c__, doublereal *s, doublereal *a,
    integer *lda);

int dlasrt_(char *id, integer *n, doublereal *d__, integer *info);

int dlassq_(integer *n, doublereal *x, integer *incx, doublereal *scale, doublereal *sumsq);

int dlasv2_(doublereal *f, doublereal *g, doublereal *h__, doublereal *ssmin, doublereal *ssmax, doublereal *snr,
    doublereal *csr, doublereal *snl, doublereal *csl);

int dlaswp_(integer *n, doublereal *a, integer *lda, integer *k1, integer *k2, integer *ipiv, integer *incx);

int dlasy2_(logical *ltranl, logical *ltranr, integer *isgn, integer *n1, integer *n2, doublereal *tl, integer *ldtl,
    doublereal *tr, integer *ldtr, doublereal *b, integer *ldb, doublereal *scale, doublereal *x, integer *ldx,
    doublereal *xnorm, integer *info);

int dlasyf_(char *uplo, integer *n, integer *nb, integer *kb, doublereal *a, integer *lda, integer *ipiv, doublereal *w,
    integer *ldw, integer *info);

int dlat2s_(char *uplo, integer *n, doublereal *a, integer *lda, real *sa, integer *ldsa, integer *info);

int dlatbs_(char *uplo, char *trans, char *diag, char *normin, integer *n, integer *kd, doublereal *ab, integer *ldab,
    doublereal *x, doublereal *scale, doublereal *cnorm, integer *info);

int dlatdf_(integer *ijob, integer *n, doublereal *z__, integer *ldz, doublereal *rhs, doublereal *rdsum,
    doublereal *rdscal, integer *ipiv, integer *jpiv);

int dlatps_(char *uplo, char *trans, char *diag, char *normin, integer *n, doublereal *ap, doublereal *x,
    doublereal *scale, doublereal *cnorm, integer *info);

int dlatrd_(char *uplo, integer *n, integer *nb, doublereal *a, integer *lda, doublereal *e, doublereal *tau,
    doublereal *w, integer *ldw);

int dlatrs_(char *uplo, char *trans, char *diag, char *normin, integer *n, doublereal *a, integer *lda, doublereal *x,
    doublereal *scale, doublereal *cnorm, integer *info);

int dlatrz_(integer *m, integer *n, integer *l, doublereal *a, integer *lda, doublereal *tau, doublereal *work);

int dlatzm_(char *side, integer *m, integer *n, doublereal *v, integer *incv, doublereal *tau, doublereal *c1,
    doublereal *c2, integer *ldc, doublereal *work);

int dlauu2_(char *uplo, integer *n, doublereal *a, integer *lda, integer *info);

int dlauum_(char *uplo, integer *n, doublereal *a, integer *lda, integer *info);

int dopgtr_(char *uplo, integer *n, doublereal *ap, doublereal *tau, doublereal *q, integer *ldq, doublereal *work,
    integer *info);

int dopmtr_(char *side, char *uplo, char *trans, integer *m, integer *n, doublereal *ap, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *info);

int dorg2l_(
    integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dorg2r_(
    integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dorgbr_(char *vect, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *work, integer *lwork, integer *info);

int dorghr_(integer *n, integer *ilo, integer *ihi, doublereal *a, integer *lda, doublereal *tau, doublereal *work,
    integer *lwork, integer *info);

int dorgl2_(
    integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dorglq_(integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work,
    integer *lwork, integer *info);

int dorgql_(integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work,
    integer *lwork, integer *info);

int dorgqr_(integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work,
    integer *lwork, integer *info);

int dorgr2_(
    integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *info);

int dorgrq_(integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau, doublereal *work,
    integer *lwork, integer *info);

int dorgtr_(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
    integer *info);

int dorm2l_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *info);

int dorm2r_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *info);

int dormbr_(char *vect, char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda,
    doublereal *tau, doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dormhr_(char *side, char *trans, integer *m, integer *n, integer *ilo, integer *ihi, doublereal *a, integer *lda,
    doublereal *tau, doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dorml2_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *info);

int dormlq_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dormql_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dormqr_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dormr2_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *info);

int dormr3_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, doublereal *a, integer *lda,
    doublereal *tau, doublereal *c__, integer *ldc, doublereal *work, integer *info);

int dormrq_(char *side, char *trans, integer *m, integer *n, integer *k, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dormrz_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, doublereal *a, integer *lda,
    doublereal *tau, doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dormtr_(char *side, char *uplo, char *trans, integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau,
    doublereal *c__, integer *ldc, doublereal *work, integer *lwork, integer *info);

int dpbcon_(char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, doublereal *anorm, doublereal *rcond,
    doublereal *work, integer *iwork, integer *info);

int dpbequ_(char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, doublereal *s, doublereal *scond,
    doublereal *amax, integer *info);

int dpbrfs_(char *uplo, integer *n, integer *kd, integer *nrhs, doublereal *ab, integer *ldab, doublereal *afb,
    integer *ldafb, doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublereal *work, integer *iwork, integer *info);

int dpbstf_(char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, integer *info);

int dpbsv_(char *uplo, integer *n, integer *kd, integer *nrhs, doublereal *ab, integer *ldab, doublereal *b,
    integer *ldb, integer *info);

int dpbsvx_(char *fact, char *uplo, integer *n, integer *kd, integer *nrhs, doublereal *ab, integer *ldab,
    doublereal *afb, integer *ldafb, char *equed, doublereal *s, doublereal *b, integer *ldb, doublereal *x,
    integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork,
    integer *info);

int dpbtf2_(char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, integer *info);

int dpbtrf_(char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, integer *info);

int dpbtrs_(char *uplo, integer *n, integer *kd, integer *nrhs, doublereal *ab, integer *ldab, doublereal *b,
    integer *ldb, integer *info);

int dpftrf_(char *transr, char *uplo, integer *n, doublereal *a, integer *info);

int dpftri_(char *transr, char *uplo, integer *n, doublereal *a, integer *info);

int dpftrs_(
    char *transr, char *uplo, integer *n, integer *nrhs, doublereal *a, doublereal *b, integer *ldb, integer *info);

int dpocon_(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *anorm, doublereal *rcond, doublereal *work,
    integer *iwork, integer *info);

int dpoequ_(integer *n, doublereal *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax, integer *info);

int dpoequb_(
    integer *n, doublereal *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax, integer *info);

int dporfs_(char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af, integer *ldaf,
    doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work,
    integer *iwork, integer *info);

int dporfsx_(char *uplo, char *equed, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, doublereal *s, doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *rcond,
    doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams,
    doublereal *params, doublereal *work, integer *iwork, integer *info);

int dposv_(
    char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb, integer *info);

int dposvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, char *equed, doublereal *s, doublereal *b, integer *ldb, doublereal *x, integer *ldx,
    doublereal *rcond, doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork, integer *info);

int dposvxx_(char *fact, char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, char *equed, doublereal *s, doublereal *b, integer *ldb, doublereal *x, integer *ldx,
    doublereal *rcond, doublereal *rpvgrw, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublereal *work, integer *iwork, integer *info);

int dpotf2_(char *uplo, integer *n, doublereal *a, integer *lda, integer *info);

int dpotrf_(char *uplo, integer *n, doublereal *a, integer *lda, integer *info);

int dpotri_(char *uplo, integer *n, doublereal *a, integer *lda, integer *info);

int dpotrs_(
    char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb, integer *info);

int dppcon_(char *uplo, integer *n, doublereal *ap, doublereal *anorm, doublereal *rcond, doublereal *work,
    integer *iwork, integer *info);

int dppequ_(char *uplo, integer *n, doublereal *ap, doublereal *s, doublereal *scond, doublereal *amax, integer *info);

int dpprfs_(char *uplo, integer *n, integer *nrhs, doublereal *ap, doublereal *afp, doublereal *b, integer *ldb,
    doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork, integer *info);

int dppsv_(char *uplo, integer *n, integer *nrhs, doublereal *ap, doublereal *b, integer *ldb, integer *info);

int dppsvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublereal *ap, doublereal *afp, char *equed,
    doublereal *s, doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *rcond, doublereal *ferr,
    doublereal *berr, doublereal *work, integer *iwork, integer *info);

int dpptrf_(char *uplo, integer *n, doublereal *ap, integer *info);

int dpptri_(char *uplo, integer *n, doublereal *ap, integer *info);

int dpptrs_(char *uplo, integer *n, integer *nrhs, doublereal *ap, doublereal *b, integer *ldb, integer *info);

int dpstf2_(char *uplo, integer *n, doublereal *a, integer *lda, integer *piv, integer *rank, doublereal *tol,
    doublereal *work, integer *info);

int dpstrf_(char *uplo, integer *n, doublereal *a, integer *lda, integer *piv, integer *rank, doublereal *tol,
    doublereal *work, integer *info);

int dptcon_(
    integer *n, doublereal *d__, doublereal *e, doublereal *anorm, doublereal *rcond, doublereal *work, integer *info);

int dpteqr_(char *compz, integer *n, doublereal *d__, doublereal *e, doublereal *z__, integer *ldz, doublereal *work,
    integer *info);

int dptrfs_(integer *n, integer *nrhs, doublereal *d__, doublereal *e, doublereal *df, doublereal *ef, doublereal *b,
    integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work, integer *info);

int dptsv_(integer *n, integer *nrhs, doublereal *d__, doublereal *e, doublereal *b, integer *ldb, integer *info);

int dptsvx_(char *fact, integer *n, integer *nrhs, doublereal *d__, doublereal *e, doublereal *df, doublereal *ef,
    doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr,
    doublereal *work, integer *info);

int dpttrf_(integer *n, doublereal *d__, doublereal *e, integer *info);

int dpttrs_(integer *n, integer *nrhs, doublereal *d__, doublereal *e, doublereal *b, integer *ldb, integer *info);

int dptts2_(integer *n, integer *nrhs, doublereal *d__, doublereal *e, doublereal *b, integer *ldb);

int drscl_(integer *n, doublereal *sa, doublereal *sx, integer *incx);

int dsbev_(char *jobz, char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, doublereal *w,
    doublereal *z__, integer *ldz, doublereal *work, integer *info);

int dsbevd_(char *jobz, char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, doublereal *w,
    doublereal *z__, integer *ldz, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dsbevx_(char *jobz, char *range, char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, doublereal *q,
    integer *ldq, doublereal *vl, doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m,
    doublereal *w, doublereal *z__, integer *ldz, doublereal *work, integer *iwork, integer *ifail, integer *info);

int dsbgst_(char *vect, char *uplo, integer *n, integer *ka, integer *kb, doublereal *ab, integer *ldab, doublereal *bb,
    integer *ldbb, doublereal *x, integer *ldx, doublereal *work, integer *info);

int dsbgv_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, doublereal *ab, integer *ldab, doublereal *bb,
    integer *ldbb, doublereal *w, doublereal *z__, integer *ldz, doublereal *work, integer *info);

int dsbgvd_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, doublereal *ab, integer *ldab, doublereal *bb,
    integer *ldbb, doublereal *w, doublereal *z__, integer *ldz, doublereal *work, integer *lwork, integer *iwork,
    integer *liwork, integer *info);

int dsbgvx_(char *jobz, char *range, char *uplo, integer *n, integer *ka, integer *kb, doublereal *ab, integer *ldab,
    doublereal *bb, integer *ldbb, doublereal *q, integer *ldq, doublereal *vl, doublereal *vu, integer *il,
    integer *iu, doublereal *abstol, integer *m, doublereal *w, doublereal *z__, integer *ldz, doublereal *work,
    integer *iwork, integer *ifail, integer *info);

int dsbtrd_(char *vect, char *uplo, integer *n, integer *kd, doublereal *ab, integer *ldab, doublereal *d__,
    doublereal *e, doublereal *q, integer *ldq, doublereal *work, integer *info);

int dsfrk_(char *transr, char *uplo, char *trans, integer *n, integer *k, doublereal *alpha, doublereal *a,
    integer *lda, doublereal *beta, doublereal *c__);

int dsgesv_(integer *n, integer *nrhs, doublereal *a, integer *lda, integer *ipiv, doublereal *b, integer *ldb,
    doublereal *x, integer *ldx, doublereal *work, real *swork, integer *iter, integer *info);

int dspcon_(char *uplo, integer *n, doublereal *ap, integer *ipiv, doublereal *anorm, doublereal *rcond,
    doublereal *work, integer *iwork, integer *info);

int dspev_(char *jobz, char *uplo, integer *n, doublereal *ap, doublereal *w, doublereal *z__, integer *ldz,
    doublereal *work, integer *info);

int dspevd_(char *jobz, char *uplo, integer *n, doublereal *ap, doublereal *w, doublereal *z__, integer *ldz,
    doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dspevx_(char *jobz, char *range, char *uplo, integer *n, doublereal *ap, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublereal *z__, integer *ldz,
    doublereal *work, integer *iwork, integer *ifail, integer *info);

int dspgst_(integer *itype, char *uplo, integer *n, doublereal *ap, doublereal *bp, integer *info);

int dspgv_(integer *itype, char *jobz, char *uplo, integer *n, doublereal *ap, doublereal *bp, doublereal *w,
    doublereal *z__, integer *ldz, doublereal *work, integer *info);

int dspgvd_(integer *itype, char *jobz, char *uplo, integer *n, doublereal *ap, doublereal *bp, doublereal *w,
    doublereal *z__, integer *ldz, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dspgvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, doublereal *ap, doublereal *bp,
    doublereal *vl, doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w,
    doublereal *z__, integer *ldz, doublereal *work, integer *iwork, integer *ifail, integer *info);

int dsposv_(char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *x, integer *ldx, doublereal *work, real *swork, integer *iter, integer *info);

int dsprfs_(char *uplo, integer *n, integer *nrhs, doublereal *ap, doublereal *afp, integer *ipiv, doublereal *b,
    integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork,
    integer *info);

int dspsv_(
    char *uplo, integer *n, integer *nrhs, doublereal *ap, integer *ipiv, doublereal *b, integer *ldb, integer *info);

int dspsvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublereal *ap, doublereal *afp, integer *ipiv,
    doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr,
    doublereal *work, integer *iwork, integer *info);

int dsptrd_(char *uplo, integer *n, doublereal *ap, doublereal *d__, doublereal *e, doublereal *tau, integer *info);

int dsptrf_(char *uplo, integer *n, doublereal *ap, integer *ipiv, integer *info);

int dsptri_(char *uplo, integer *n, doublereal *ap, integer *ipiv, doublereal *work, integer *info);

int dsptrs_(
    char *uplo, integer *n, integer *nrhs, doublereal *ap, integer *ipiv, doublereal *b, integer *ldb, integer *info);

int dstebz_(char *range, char *order, integer *n, doublereal *vl, doublereal *vu, integer *il, integer *iu,
    doublereal *abstol, doublereal *d__, doublereal *e, integer *m, integer *nsplit, doublereal *w, integer *iblock,
    integer *isplit, doublereal *work, integer *iwork, integer *info);

int dstedc_(char *compz, integer *n, doublereal *d__, doublereal *e, doublereal *z__, integer *ldz, doublereal *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int dstegr_(char *jobz, char *range, integer *n, doublereal *d__, doublereal *e, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublereal *z__, integer *ldz,
    integer *isuppz, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dstein_(integer *n, doublereal *d__, doublereal *e, integer *m, doublereal *w, integer *iblock, integer *isplit,
    doublereal *z__, integer *ldz, doublereal *work, integer *iwork, integer *ifail, integer *info);

int dstemr_(char *jobz, char *range, integer *n, doublereal *d__, doublereal *e, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, integer *m, doublereal *w, doublereal *z__, integer *ldz, integer *nzc, integer *isuppz,
    logical *tryrac, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dsteqr_(char *compz, integer *n, doublereal *d__, doublereal *e, doublereal *z__, integer *ldz, doublereal *work,
    integer *info);

int dsterf_(integer *n, doublereal *d__, doublereal *e, integer *info);

int dstev_(char *jobz, integer *n, doublereal *d__, doublereal *e, doublereal *z__, integer *ldz, doublereal *work,
    integer *info);

int dstevd_(char *jobz, integer *n, doublereal *d__, doublereal *e, doublereal *z__, integer *ldz, doublereal *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int dstevr_(char *jobz, char *range, integer *n, doublereal *d__, doublereal *e, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublereal *z__, integer *ldz,
    integer *isuppz, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dstevx_(char *jobz, char *range, integer *n, doublereal *d__, doublereal *e, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublereal *z__, integer *ldz,
    doublereal *work, integer *iwork, integer *ifail, integer *info);

int dsycon_(char *uplo, integer *n, doublereal *a, integer *lda, integer *ipiv, doublereal *anorm, doublereal *rcond,
    doublereal *work, integer *iwork, integer *info);

int dsyequb_(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax,
    doublereal *work, integer *info);

int dsyev_(char *jobz, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *w, doublereal *work,
    integer *lwork, integer *info);

int dsyevd_(char *jobz, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *w, doublereal *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int dsyevr_(char *jobz, char *range, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *vl,
    doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublereal *z__,
    integer *ldz, integer *isuppz, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dsyevx_(char *jobz, char *range, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *vl,
    doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublereal *z__,
    integer *ldz, doublereal *work, integer *lwork, integer *iwork, integer *ifail, integer *info);

int dsygs2_(
    integer *itype, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb, integer *info);

int dsygst_(
    integer *itype, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb, integer *info);

int dsygv_(integer *itype, char *jobz, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *w, doublereal *work, integer *lwork, integer *info);

int dsygvd_(integer *itype, char *jobz, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *w, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dsygvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *vl, doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m,
    doublereal *w, doublereal *z__, integer *ldz, doublereal *work, integer *lwork, integer *iwork, integer *ifail,
    integer *info);

int dsyrfs_(char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af, integer *ldaf,
    integer *ipiv, doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublereal *work, integer *iwork, integer *info);

int dsyrfsx_(char *uplo, char *equed, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, integer *ipiv, doublereal *s, doublereal *b, integer *ldb, doublereal *x, integer *ldx,
    doublereal *rcond, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublereal *work, integer *iwork, integer *info);

int dsysv_(char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, integer *ipiv, doublereal *b,
    integer *ldb, doublereal *work, integer *lwork, integer *info);

int dsysvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, integer *ipiv, doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *rcond,
    doublereal *ferr, doublereal *berr, doublereal *work, integer *lwork, integer *iwork, integer *info);

int dsysvxx_(char *fact, char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *af,
    integer *ldaf, integer *ipiv, char *equed, doublereal *s, doublereal *b, integer *ldb, doublereal *x, integer *ldx,
    doublereal *rcond, doublereal *rpvgrw, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublereal *work, integer *iwork, integer *info);

int dsytd2_(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *d__, doublereal *e, doublereal *tau,
    integer *info);

int dsytf2_(char *uplo, integer *n, doublereal *a, integer *lda, integer *ipiv, integer *info);

int dsytrd_(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *d__, doublereal *e, doublereal *tau,
    doublereal *work, integer *lwork, integer *info);

int dsytrf_(char *uplo, integer *n, doublereal *a, integer *lda, integer *ipiv, doublereal *work, integer *lwork,
    integer *info);

int dsytri_(char *uplo, integer *n, doublereal *a, integer *lda, integer *ipiv, doublereal *work, integer *info);

int dsytrs_(char *uplo, integer *n, integer *nrhs, doublereal *a, integer *lda, integer *ipiv, doublereal *b,
    integer *ldb, integer *info);

int dtbcon_(char *norm, char *uplo, char *diag, integer *n, integer *kd, doublereal *ab, integer *ldab,
    doublereal *rcond, doublereal *work, integer *iwork, integer *info);

int dtbrfs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, doublereal *ab, integer *ldab,
    doublereal *b, integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work,
    integer *iwork, integer *info);

int dtbtrs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, doublereal *ab, integer *ldab,
    doublereal *b, integer *ldb, integer *info);

int dtfsm_(char *transr, char *side, char *uplo, char *trans, char *diag, integer *m, integer *n, doublereal *alpha,
    doublereal *a, doublereal *b, integer *ldb);

int dtftri_(char *transr, char *uplo, char *diag, integer *n, doublereal *a, integer *info);

int dtfttp_(char *transr, char *uplo, integer *n, doublereal *arf, doublereal *ap, integer *info);

int dtfttr_(char *transr, char *uplo, integer *n, doublereal *arf, doublereal *a, integer *lda, integer *info);

int dtgevc_(char *side, char *howmny, logical *select, integer *n, doublereal *s, integer *lds, doublereal *p,
    integer *ldp, doublereal *vl, integer *ldvl, doublereal *vr, integer *ldvr, integer *mm, integer *m,
    doublereal *work, integer *info);

int dtgex2_(logical *wantq, logical *wantz, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *q, integer *ldq, doublereal *z__, integer *ldz, integer *j1, integer *n1, integer *n2, doublereal *work,
    integer *lwork, integer *info);

int dtgexc_(logical *wantq, logical *wantz, integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb,
    doublereal *q, integer *ldq, doublereal *z__, integer *ldz, integer *ifst, integer *ilst, doublereal *work,
    integer *lwork, integer *info);

int dtgsen_(integer *ijob, logical *wantq, logical *wantz, logical *select, integer *n, doublereal *a, integer *lda,
    doublereal *b, integer *ldb, doublereal *alphar, doublereal *alphai, doublereal *beta, doublereal *q, integer *ldq,
    doublereal *z__, integer *ldz, integer *m, doublereal *pl, doublereal *pr, doublereal *dif, doublereal *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int dtgsja_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, integer *k, integer *l,
    doublereal *a, integer *lda, doublereal *b, integer *ldb, doublereal *tola, doublereal *tolb, doublereal *alpha,
    doublereal *beta, doublereal *u, integer *ldu, doublereal *v, integer *ldv, doublereal *q, integer *ldq,
    doublereal *work, integer *ncycle, integer *info);

int dtgsna_(char *job, char *howmny, logical *select, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *vl, integer *ldvl, doublereal *vr, integer *ldvr, doublereal *s, doublereal *dif,
    integer *mm, integer *m, doublereal *work, integer *lwork, integer *iwork, integer *info);

int dtgsy2_(char *trans, integer *ijob, integer *m, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *c__, integer *ldc, doublereal *d__, integer *ldd, doublereal *e, integer *lde,
    doublereal *f, integer *ldf, doublereal *scale, doublereal *rdsum, doublereal *rdscal, integer *iwork, integer *pq,
    integer *info);

int dtgsyl_(char *trans, integer *ijob, integer *m, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *c__, integer *ldc, doublereal *d__, integer *ldd, doublereal *e, integer *lde,
    doublereal *f, integer *ldf, doublereal *scale, doublereal *dif, doublereal *work, integer *lwork, integer *iwork,
    integer *info);

int dtpcon_(char *norm, char *uplo, char *diag, integer *n, doublereal *ap, doublereal *rcond, doublereal *work,
    integer *iwork, integer *info);

int dtprfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublereal *ap, doublereal *b, integer *ldb,
    doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork, integer *info);

int dtptri_(char *uplo, char *diag, integer *n, doublereal *ap, integer *info);

int dtptrs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublereal *ap, doublereal *b, integer *ldb,
    integer *info);

int dtpttf_(char *transr, char *uplo, integer *n, doublereal *ap, doublereal *arf, integer *info);

int dtpttr_(char *uplo, integer *n, doublereal *ap, doublereal *a, integer *lda, integer *info);

int dtrcon_(char *norm, char *uplo, char *diag, integer *n, doublereal *a, integer *lda, doublereal *rcond,
    doublereal *work, integer *iwork, integer *info);

int dtrevc_(char *side, char *howmny, logical *select, integer *n, doublereal *t, integer *ldt, doublereal *vl,
    integer *ldvl, doublereal *vr, integer *ldvr, integer *mm, integer *m, doublereal *work, integer *info);

int dtrexc_(char *compq, integer *n, doublereal *t, integer *ldt, doublereal *q, integer *ldq, integer *ifst,
    integer *ilst, doublereal *work, integer *info);

int dtrrfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *x, integer *ldx, doublereal *ferr, doublereal *berr, doublereal *work, integer *iwork,
    integer *info);

int dtrsen_(char *job, char *compq, logical *select, integer *n, doublereal *t, integer *ldt, doublereal *q,
    integer *ldq, doublereal *wr, doublereal *wi, integer *m, doublereal *s, doublereal *sep, doublereal *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int dtrsna_(char *job, char *howmny, logical *select, integer *n, doublereal *t, integer *ldt, doublereal *vl,
    integer *ldvl, doublereal *vr, integer *ldvr, doublereal *s, doublereal *sep, integer *mm, integer *m,
    doublereal *work, integer *ldwork, integer *iwork, integer *info);

int dtrsyl_(char *trana, char *tranb, integer *isgn, integer *m, integer *n, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, doublereal *c__, integer *ldc, doublereal *scale, integer *info);

int dtrti2_(char *uplo, char *diag, integer *n, doublereal *a, integer *lda, integer *info);

int dtrtri_(char *uplo, char *diag, integer *n, doublereal *a, integer *lda, integer *info);

int dtrtrs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b,
    integer *ldb, integer *info);

int dtrttf_(char *transr, char *uplo, integer *n, doublereal *a, integer *lda, doublereal *arf, integer *info);

int dtrttp_(char *uplo, integer *n, doublereal *a, integer *lda, doublereal *ap, integer *info);

int dtzrqf_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, integer *info);

int dtzrzf_(integer *m, integer *n, doublereal *a, integer *lda, doublereal *tau, doublereal *work, integer *lwork,
    integer *info);

doublereal dzsum1_(integer *n, doublecomplex *cx, integer *incx);

integer icmax1_(integer *n, complex *cx, integer *incx);

integer ieeeck_(integer *ispec, real *zero, real *one);

integer ilaclc_(integer *m, integer *n, complex *a, integer *lda);

integer ilaclr_(integer *m, integer *n, complex *a, integer *lda);

integer iladiag_(char *diag);

integer iladlc_(integer *m, integer *n, doublereal *a, integer *lda);

integer iladlr_(integer *m, integer *n, doublereal *a, integer *lda);

integer ilaenv_(integer *ispec, char *name__, char *opts, integer *n1, integer *n2, integer *n3, integer *n4);

integer ilaprec_(char *prec);

integer ilaslc_(integer *m, integer *n, real *a, integer *lda);

integer ilaslr_(integer *m, integer *n, real *a, integer *lda);

integer ilatrans_(char *trans);

integer ilauplo_(char *uplo);

int ilaver_(integer *vers_major__, integer *vers_minor__, integer *vers_patch__);

integer ilazlc_(integer *m, integer *n, doublecomplex *a, integer *lda);

integer ilazlr_(integer *m, integer *n, doublecomplex *a, integer *lda);

integer iparmq_(integer *ispec, char *name__, char *opts, integer *n, integer *ilo, integer *ihi, integer *lwork);

integer izmax1_(integer *n, doublecomplex *cx, integer *incx);

logical lsamen_(integer *n, char *ca, char *cb);

integer smaxloc_(real *a, integer *dimm);

int sbdsdc_(char *uplo, char *compq, integer *n, real *d__, real *e, real *u, integer *ldu, real *vt, integer *ldvt,
    real *q, integer *iq, real *work, integer *iwork, integer *info);

int sbdsqr_(char *uplo, integer *n, integer *ncvt, integer *nru, integer *ncc, real *d__, real *e, real *vt,
    integer *ldvt, real *u, integer *ldu, real *c__, integer *ldc, real *work, integer *info);

doublereal scsum1_(integer *n, complex *cx, integer *incx);

int sdisna_(char *job, integer *m, integer *n, real *d__, real *sep, integer *info);

int sgbbrd_(char *vect, integer *m, integer *n, integer *ncc, integer *kl, integer *ku, real *ab, integer *ldab,
    real *d__, real *e, real *q, integer *ldq, real *pt, integer *ldpt, real *c__, integer *ldc, real *work,
    integer *info);

int sgbcon_(char *norm, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, integer *ipiv, real *anorm,
    real *rcond, real *work, integer *iwork, integer *info);

int sgbequ_(integer *m, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, real *r__, real *c__,
    real *rowcnd, real *colcnd, real *amax, integer *info);

int sgbequb_(integer *m, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, real *r__, real *c__,
    real *rowcnd, real *colcnd, real *amax, integer *info);

int sgbrfs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, real *ab, integer *ldab, real *afb,
    integer *ldafb, integer *ipiv, real *b, integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work,
    integer *iwork, integer *info);

int sgbrfsx_(char *trans, char *equed, integer *n, integer *kl, integer *ku, integer *nrhs, real *ab, integer *ldab,
    real *afb, integer *ldafb, integer *ipiv, real *r__, real *c__, real *b, integer *ldb, real *x, integer *ldx,
    real *rcond, real *berr, integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams,
    real *params, real *work, integer *iwork, integer *info);

int sgbsv_(integer *n, integer *kl, integer *ku, integer *nrhs, real *ab, integer *ldab, integer *ipiv, real *b,
    integer *ldb, integer *info);

int sgbsvx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, real *ab, integer *ldab,
    real *afb, integer *ldafb, integer *ipiv, char *equed, real *r__, real *c__, real *b, integer *ldb, real *x,
    integer *ldx, real *rcond, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int sgbsvxx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, real *ab, integer *ldab,
    real *afb, integer *ldafb, integer *ipiv, char *equed, real *r__, real *c__, real *b, integer *ldb, real *x,
    integer *ldx, real *rcond, real *rpvgrw, real *berr, integer *n_err_bnds__, real *err_bnds_norm__,
    real *err_bnds_comp__, integer *nparams, real *params, real *work, integer *iwork, integer *info);

int sgbtf2_(integer *m, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, integer *ipiv, integer *info);

int sgbtrf_(integer *m, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, integer *ipiv, integer *info);

int sgbtrs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, real *ab, integer *ldab, integer *ipiv,
    real *b, integer *ldb, integer *info);

int sgebak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, real *scale, integer *m, real *v,
    integer *ldv, integer *info);

int sgebal_(char *job, integer *n, real *a, integer *lda, integer *ilo, integer *ihi, real *scale, integer *info);

int sgebd2_(integer *m, integer *n, real *a, integer *lda, real *d__, real *e, real *tauq, real *taup, real *work,
    integer *info);

int sgebrd_(integer *m, integer *n, real *a, integer *lda, real *d__, real *e, real *tauq, real *taup, real *work,
    integer *lwork, integer *info);

int sgecon_(
    char *norm, integer *n, real *a, integer *lda, real *anorm, real *rcond, real *work, integer *iwork, integer *info);

int sgeequ_(integer *m, integer *n, real *a, integer *lda, real *r__, real *c__, real *rowcnd, real *colcnd, real *amax,
    integer *info);

int sgeequb_(integer *m, integer *n, real *a, integer *lda, real *r__, real *c__, real *rowcnd, real *colcnd,
    real *amax, integer *info);

int sgees_(char *jobvs, char *sort, L_fp select, integer *n, real *a, integer *lda, integer *sdim, real *wr, real *wi,
    real *vs, integer *ldvs, real *work, integer *lwork, logical *bwork, integer *info);

int sgeesx_(char *jobvs, char *sort, L_fp select, char *sense, integer *n, real *a, integer *lda, integer *sdim,
    real *wr, real *wi, real *vs, integer *ldvs, real *rconde, real *rcondv, real *work, integer *lwork, integer *iwork,
    integer *liwork, logical *bwork, integer *info);

int sgeev_(char *jobvl, char *jobvr, integer *n, real *a, integer *lda, real *wr, real *wi, real *vl, integer *ldvl,
    real *vr, integer *ldvr, real *work, integer *lwork, integer *info);

int sgeevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, real *a, integer *lda, real *wr, real *wi,
    real *vl, integer *ldvl, real *vr, integer *ldvr, integer *ilo, integer *ihi, real *scale, real *abnrm,
    real *rconde, real *rcondv, real *work, integer *lwork, integer *iwork, integer *info);

int sgegs_(char *jobvsl, char *jobvsr, integer *n, real *a, integer *lda, real *b, integer *ldb, real *alphar,
    real *alphai, real *beta, real *vsl, integer *ldvsl, real *vsr, integer *ldvsr, real *work, integer *lwork,
    integer *info);

int sgegv_(char *jobvl, char *jobvr, integer *n, real *a, integer *lda, real *b, integer *ldb, real *alphar,
    real *alphai, real *beta, real *vl, integer *ldvl, real *vr, integer *ldvr, real *work, integer *lwork,
    integer *info);

int sgehd2_(integer *n, integer *ilo, integer *ihi, real *a, integer *lda, real *tau, real *work, integer *info);

int sgehrd_(integer *n, integer *ilo, integer *ihi, real *a, integer *lda, real *tau, real *work, integer *lwork,
    integer *info);

int sgejsv_(char *joba, char *jobu, char *jobv, char *jobr, char *jobt, char *jobp, integer *m, integer *n, real *a,
    integer *lda, real *sva, real *u, integer *ldu, real *v, integer *ldv, real *work, integer *lwork, integer *iwork,
    integer *info);

int sgelq2_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *info);

int sgelqf_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sgels_(char *trans, integer *m, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, real *work,
    integer *lwork, integer *info);

int sgelsd_(integer *m, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, real *s, real *rcond,
    integer *rank, real *work, integer *lwork, integer *iwork, integer *info);

int sgelss_(integer *m, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, real *s, real *rcond,
    integer *rank, real *work, integer *lwork, integer *info);

int sgelsx_(integer *m, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, integer *jpvt,
    real *rcond, integer *rank, real *work, integer *info);

int sgelsy_(integer *m, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, integer *jpvt,
    real *rcond, integer *rank, real *work, integer *lwork, integer *info);

int sgeql2_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *info);

int sgeqlf_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sgeqp3_(
    integer *m, integer *n, real *a, integer *lda, integer *jpvt, real *tau, real *work, integer *lwork, integer *info);

int sgeqpf_(integer *m, integer *n, real *a, integer *lda, integer *jpvt, real *tau, real *work, integer *info);

int sgeqr2_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *info);

int sgeqrf_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sgerfs_(char *trans, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf, integer *ipiv,
    real *b, integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int sgerfsx_(char *trans, char *equed, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    integer *ipiv, real *r__, real *c__, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *berr,
    integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, real *work,
    integer *iwork, integer *info);

int sgerq2_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *info);

int sgerqf_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sgesc2_(integer *n, real *a, integer *lda, real *rhs, integer *ipiv, integer *jpiv, real *scale);

int sgesdd_(char *jobz, integer *m, integer *n, real *a, integer *lda, real *s, real *u, integer *ldu, real *vt,
    integer *ldvt, real *work, integer *lwork, integer *iwork, integer *info);

int sgesv_(integer *n, integer *nrhs, real *a, integer *lda, integer *ipiv, real *b, integer *ldb, integer *info);

int sgesvd_(char *jobu, char *jobvt, integer *m, integer *n, real *a, integer *lda, real *s, real *u, integer *ldu,
    real *vt, integer *ldvt, real *work, integer *lwork, integer *info);

int sgesvj_(char *joba, char *jobu, char *jobv, integer *m, integer *n, real *a, integer *lda, real *sva, integer *mv,
    real *v, integer *ldv, real *work, integer *lwork, integer *info);

int sgesvx_(char *fact, char *trans, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    integer *ipiv, char *equed, real *r__, real *c__, real *b, integer *ldb, real *x, integer *ldx, real *rcond,
    real *ferr, real *berr, real *work, integer *iwork, integer *info);

int sgesvxx_(char *fact, char *trans, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    integer *ipiv, char *equed, real *r__, real *c__, real *b, integer *ldb, real *x, integer *ldx, real *rcond,
    real *rpvgrw, real *berr, integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams,
    real *params, real *work, integer *iwork, integer *info);

int sgetc2_(integer *n, real *a, integer *lda, integer *ipiv, integer *jpiv, integer *info);

int sgetf2_(integer *m, integer *n, real *a, integer *lda, integer *ipiv, integer *info);

int sgetrf_(integer *m, integer *n, real *a, integer *lda, integer *ipiv, integer *info);

int sgetri_(integer *n, real *a, integer *lda, integer *ipiv, real *work, integer *lwork, integer *info);

int sgetrs_(
    char *trans, integer *n, integer *nrhs, real *a, integer *lda, integer *ipiv, real *b, integer *ldb, integer *info);

int sggbak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, real *lscale, real *rscale, integer *m,
    real *v, integer *ldv, integer *info);

int sggbal_(char *job, integer *n, real *a, integer *lda, real *b, integer *ldb, integer *ilo, integer *ihi,
    real *lscale, real *rscale, real *work, integer *info);

int sgges_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, integer *n, real *a, integer *lda, real *b,
    integer *ldb, integer *sdim, real *alphar, real *alphai, real *beta, real *vsl, integer *ldvsl, real *vsr,
    integer *ldvsr, real *work, integer *lwork, logical *bwork, integer *info);

int sggesx_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, char *sense, integer *n, real *a, integer *lda,
    real *b, integer *ldb, integer *sdim, real *alphar, real *alphai, real *beta, real *vsl, integer *ldvsl, real *vsr,
    integer *ldvsr, real *rconde, real *rcondv, real *work, integer *lwork, integer *iwork, integer *liwork,
    logical *bwork, integer *info);

int sggev_(char *jobvl, char *jobvr, integer *n, real *a, integer *lda, real *b, integer *ldb, real *alphar,
    real *alphai, real *beta, real *vl, integer *ldvl, real *vr, integer *ldvr, real *work, integer *lwork,
    integer *info);

int sggevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, real *a, integer *lda, real *b,
    integer *ldb, real *alphar, real *alphai, real *beta, real *vl, integer *ldvl, real *vr, integer *ldvr,
    integer *ilo, integer *ihi, real *lscale, real *rscale, real *abnrm, real *bbnrm, real *rconde, real *rcondv,
    real *work, integer *lwork, integer *iwork, logical *bwork, integer *info);

int sggglm_(integer *n, integer *m, integer *p, real *a, integer *lda, real *b, integer *ldb, real *d__, real *x,
    real *y, real *work, integer *lwork, integer *info);

int sgghrd_(char *compq, char *compz, integer *n, integer *ilo, integer *ihi, real *a, integer *lda, real *b,
    integer *ldb, real *q, integer *ldq, real *z__, integer *ldz, integer *info);

int sgglse_(integer *m, integer *n, integer *p, real *a, integer *lda, real *b, integer *ldb, real *c__, real *d__,
    real *x, real *work, integer *lwork, integer *info);

int sggqrf_(integer *n, integer *m, integer *p, real *a, integer *lda, real *taua, real *b, integer *ldb, real *taub,
    real *work, integer *lwork, integer *info);

int sggrqf_(integer *m, integer *p, integer *n, real *a, integer *lda, real *taua, real *b, integer *ldb, real *taub,
    real *work, integer *lwork, integer *info);

int sggsvd_(char *jobu, char *jobv, char *jobq, integer *m, integer *n, integer *p, integer *k, integer *l, real *a,
    integer *lda, real *b, integer *ldb, real *alpha, real *beta, real *u, integer *ldu, real *v, integer *ldv, real *q,
    integer *ldq, real *work, integer *iwork, integer *info);

int sggsvp_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, real *a, integer *lda, real *b,
    integer *ldb, real *tola, real *tolb, integer *k, integer *l, real *u, integer *ldu, real *v, integer *ldv, real *q,
    integer *ldq, integer *iwork, real *tau, real *work, integer *info);

int sgsvj0_(char *jobv, integer *m, integer *n, real *a, integer *lda, real *d__, real *sva, integer *mv, real *v,
    integer *ldv, real *eps, real *sfmin, real *tol, integer *nsweep, real *work, integer *lwork, integer *info);

int sgsvj1_(char *jobv, integer *m, integer *n, integer *n1, real *a, integer *lda, real *d__, real *sva, integer *mv,
    real *v, integer *ldv, real *eps, real *sfmin, real *tol, integer *nsweep, real *work, integer *lwork,
    integer *info);

int sgtcon_(char *norm, integer *n, real *dl, real *d__, real *du, real *du2, integer *ipiv, real *anorm, real *rcond,
    real *work, integer *iwork, integer *info);

int sgtrfs_(char *trans, integer *n, integer *nrhs, real *dl, real *d__, real *du, real *dlf, real *df, real *duf,
    real *du2, integer *ipiv, real *b, integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work,
    integer *iwork, integer *info);

int sgtsv_(integer *n, integer *nrhs, real *dl, real *d__, real *du, real *b, integer *ldb, integer *info);

int sgtsvx_(char *fact, char *trans, integer *n, integer *nrhs, real *dl, real *d__, real *du, real *dlf, real *df,
    real *duf, real *du2, integer *ipiv, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *ferr,
    real *berr, real *work, integer *iwork, integer *info);

int sgttrf_(integer *n, real *dl, real *d__, real *du, real *du2, integer *ipiv, integer *info);

int sgttrs_(char *trans, integer *n, integer *nrhs, real *dl, real *d__, real *du, real *du2, integer *ipiv, real *b,
    integer *ldb, integer *info);

int sgtts2_(integer *itrans, integer *n, integer *nrhs, real *dl, real *d__, real *du, real *du2, integer *ipiv,
    real *b, integer *ldb);

int shgeqz_(char *job, char *compq, char *compz, integer *n, integer *ilo, integer *ihi, real *h__, integer *ldh,
    real *t, integer *ldt, real *alphar, real *alphai, real *beta, real *q, integer *ldq, real *z__, integer *ldz,
    real *work, integer *lwork, integer *info);

int shsein_(char *side, char *eigsrc, char *initv, logical *select, integer *n, real *h__, integer *ldh, real *wr,
    real *wi, real *vl, integer *ldvl, real *vr, integer *ldvr, integer *mm, integer *m, real *work, integer *ifaill,
    integer *ifailr, integer *info);

int shseqr_(char *job, char *compz, integer *n, integer *ilo, integer *ihi, real *h__, integer *ldh, real *wr, real *wi,
    real *z__, integer *ldz, real *work, integer *lwork, integer *info);

logical sisnan_(real *sin__);

int sla_gbamv__(integer *trans, integer *m, integer *n, integer *kl, integer *ku, real *alpha, real *ab, integer *ldab,
    real *x, integer *incx, real *beta, real *y, integer *incy);

doublereal sla_gbrcond__(char *trans, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, real *afb,
    integer *ldafb, integer *ipiv, integer *cmode, real *c__, integer *info, real *work, integer *iwork,
    ftnlen trans_len);

int sla_gbrfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *kl, integer *ku,
    integer *nrhs, real *ab, integer *ldab, real *afb, integer *ldafb, integer *ipiv, logical *colequ, real *c__,
    real *b, integer *ldb, real *y, integer *ldy, real *berr_out__, integer *n_norms__, real *errs_n__, real *errs_c__,
    real *res, real *ayb, real *dy, real *y_tail__, real *rcond, integer *ithresh, real *rthresh, real *dz_ub__,
    logical *ignore_cwise__, integer *info);

doublereal sla_gbrpvgrw__(
    integer *n, integer *kl, integer *ku, integer *ncols, real *ab, integer *ldab, real *afb, integer *ldafb);

int sla_geamv__(integer *trans, integer *m, integer *n, real *alpha, real *a, integer *lda, real *x, integer *incx,
    real *beta, real *y, integer *incy);

doublereal sla_gercond__(char *trans, integer *n, real *a, integer *lda, real *af, integer *ldaf, integer *ipiv,
    integer *cmode, real *c__, integer *info, real *work, integer *iwork, ftnlen trans_len);

int sla_gerfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *nrhs, real *a, integer *lda,
    real *af, integer *ldaf, integer *ipiv, logical *colequ, real *c__, real *b, integer *ldb, real *y, integer *ldy,
    real *berr_out__, integer *n_norms__, real *errs_n__, real *errs_c__, real *res, real *ayb, real *dy,
    real *y_tail__, real *rcond, integer *ithresh, real *rthresh, real *dz_ub__, logical *ignore_cwise__,
    integer *info);

int sla_lin_berr__(integer *n, integer *nz, integer *nrhs, real *res, real *ayb, real *berr);

doublereal sla_porcond__(char *uplo, integer *n, real *a, integer *lda, real *af, integer *ldaf, integer *cmode,
    real *c__, integer *info, real *work, integer *iwork, ftnlen uplo_len);

int sla_porfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af,
    integer *ldaf, logical *colequ, real *c__, real *b, integer *ldb, real *y, integer *ldy, real *berr_out__,
    integer *n_norms__, real *errs_n__, real *errs_c__, real *res, real *ayb, real *dy, real *y_tail__, real *rcond,
    integer *ithresh, real *rthresh, real *dz_ub__, logical *ignore_cwise__, integer *info, ftnlen uplo_len);

doublereal sla_porpvgrw__(
    char *uplo, integer *ncols, real *a, integer *lda, real *af, integer *ldaf, real *work, ftnlen uplo_len);

doublereal sla_rpvgrw__(integer *n, integer *ncols, real *a, integer *lda, real *af, integer *ldaf);

int sla_syamv__(integer *uplo, integer *n, real *alpha, real *a, integer *lda, real *x, integer *incx, real *beta,
    real *y, integer *incy);

doublereal sla_syrcond__(char *uplo, integer *n, real *a, integer *lda, real *af, integer *ldaf, integer *ipiv,
    integer *cmode, real *c__, integer *info, real *work, integer *iwork, ftnlen uplo_len);

int sla_syrfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af,
    integer *ldaf, integer *ipiv, logical *colequ, real *c__, real *b, integer *ldb, real *y, integer *ldy,
    real *berr_out__, integer *n_norms__, real *errs_n__, real *errs_c__, real *res, real *ayb, real *dy,
    real *y_tail__, real *rcond, integer *ithresh, real *rthresh, real *dz_ub__, logical *ignore_cwise__, integer *info,
    ftnlen uplo_len);

doublereal sla_syrpvgrw__(char *uplo, integer *n, integer *info, real *a, integer *lda, real *af, integer *ldaf,
    integer *ipiv, real *work, ftnlen uplo_len);

int sla_wwaddw__(integer *n, real *x, real *y, real *w);

int slabad_(real *small, real *large);

int slabrd_(integer *m, integer *n, integer *nb, real *a, integer *lda, real *d__, real *e, real *tauq, real *taup,
    real *x, integer *ldx, real *y, integer *ldy);

int slacn2_(integer *n, real *v, real *x, integer *isgn, real *est, integer *kase, integer *isave);

int slacon_(integer *n, real *v, real *x, integer *isgn, real *est, integer *kase);

int slacpy_(char *uplo, integer *m, integer *n, real *a, integer *lda, real *b, integer *ldb);

int sladiv_(real *a, real *b, real *c__, real *d__, real *p, real *q);

int slae2_(real *a, real *b, real *c__, real *rt1, real *rt2);

int slaebz_(integer *ijob, integer *nitmax, integer *n, integer *mmax, integer *minp, integer *nbmin, real *abstol,
    real *reltol, real *pivmin, real *d__, real *e, real *e2, integer *nval, real *ab, real *c__, integer *mout,
    integer *nab, real *work, integer *iwork, integer *info);

int slaed0_(integer *icompq, integer *qsiz, integer *n, real *d__, real *e, real *q, integer *ldq, real *qstore,
    integer *ldqs, real *work, integer *iwork, integer *info);

int slaed1_(integer *n, real *d__, real *q, integer *ldq, integer *indxq, real *rho, integer *cutpnt, real *work,
    integer *iwork, integer *info);

int slaed2_(integer *k, integer *n, integer *n1, real *d__, real *q, integer *ldq, integer *indxq, real *rho, real *z__,
    real *dlamda, real *w, real *q2, integer *indx, integer *indxc, integer *indxp, integer *coltyp, integer *info);

int slaed3_(integer *k, integer *n, integer *n1, real *d__, real *q, integer *ldq, real *rho, real *dlamda, real *q2,
    integer *indx, integer *ctot, real *w, real *s, integer *info);

int slaed4_(integer *n, integer *i__, real *d__, real *z__, real *delta, real *rho, real *dlam, integer *info);

int slaed5_(integer *i__, real *d__, real *z__, real *delta, real *rho, real *dlam);

int slaed6_(integer *kniter, logical *orgati, real *rho, real *d__, real *z__, real *finit, real *tau, integer *info);

int slaed7_(integer *icompq, integer *n, integer *qsiz, integer *tlvls, integer *curlvl, integer *curpbm, real *d__,
    real *q, integer *ldq, integer *indxq, real *rho, integer *cutpnt, real *qstore, integer *qptr, integer *prmptr,
    integer *perm, integer *givptr, integer *givcol, real *givnum, real *work, integer *iwork, integer *info);

int slaed8_(integer *icompq, integer *k, integer *n, integer *qsiz, real *d__, real *q, integer *ldq, integer *indxq,
    real *rho, integer *cutpnt, real *z__, real *dlamda, real *q2, integer *ldq2, real *w, integer *perm,
    integer *givptr, integer *givcol, real *givnum, integer *indxp, integer *indx, integer *info);

int slaed9_(integer *k, integer *kstart, integer *kstop, integer *n, real *d__, real *q, integer *ldq, real *rho,
    real *dlamda, real *w, real *s, integer *lds, integer *info);

int slaeda_(integer *n, integer *tlvls, integer *curlvl, integer *curpbm, integer *prmptr, integer *perm,
    integer *givptr, integer *givcol, real *givnum, real *q, integer *qptr, real *z__, real *ztemp, integer *info);

int slaein_(logical *rightv, logical *noinit, integer *n, real *h__, integer *ldh, real *wr, real *wi, real *vr,
    real *vi, real *b, integer *ldb, real *work, real *eps3, real *smlnum, real *bignum, integer *info);

int slaev2_(real *a, real *b, real *c__, real *rt1, real *rt2, real *cs1, real *sn1);

int slaexc_(logical *wantq, integer *n, real *t, integer *ldt, real *q, integer *ldq, integer *j1, integer *n1,
    integer *n2, real *work, integer *info);

int slag2_(real *a, integer *lda, real *b, integer *ldb, real *safmin, real *scale1, real *scale2, real *wr1, real *wr2,
    real *wi);

int slag2d_(integer *m, integer *n, real *sa, integer *ldsa, doublereal *a, integer *lda, integer *info);

int slags2_(logical *upper, real *a1, real *a2, real *a3, real *b1, real *b2, real *b3, real *csu, real *snu, real *csv,
    real *snv, real *csq, real *snq);

int slagtf_(integer *n, real *a, real *lambda, real *b, real *c__, real *tol, real *d__, integer *in, integer *info);

int slagtm_(char *trans, integer *n, integer *nrhs, real *alpha, real *dl, real *d__, real *du, real *x, integer *ldx,
    real *beta, real *b, integer *ldb);

int slagts_(
    integer *job, integer *n, real *a, real *b, real *c__, real *d__, integer *in, real *y, real *tol, integer *info);

int slagv2_(real *a, integer *lda, real *b, integer *ldb, real *alphar, real *alphai, real *beta, real *csl, real *snl,
    real *csr, real *snr);

int slahqr_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, real *h__, integer *ldh, real *wr,
    real *wi, integer *iloz, integer *ihiz, real *z__, integer *ldz, integer *info);

int slahr2_(integer *n, integer *k, integer *nb, real *a, integer *lda, real *tau, real *t, integer *ldt, real *y,
    integer *ldy);

int slahrd_(integer *n, integer *k, integer *nb, real *a, integer *lda, real *tau, real *t, integer *ldt, real *y,
    integer *ldy);

int slaic1_(integer *job, integer *j, real *x, real *sest, real *w, real *gamma, real *sestpr, real *s, real *c__);

logical slaisnan_(real *sin1, real *sin2);

int slaln2_(logical *ltrans, integer *na, integer *nw, real *smin, real *ca, real *a, integer *lda, real *d1, real *d2,
    real *b, integer *ldb, real *wr, real *wi, real *x, integer *ldx, real *scale, real *xnorm, integer *info);

int slals0_(integer *icompq, integer *nl, integer *nr, integer *sqre, integer *nrhs, real *b, integer *ldb, real *bx,
    integer *ldbx, integer *perm, integer *givptr, integer *givcol, integer *ldgcol, real *givnum, integer *ldgnum,
    real *poles, real *difl, real *difr, real *z__, integer *k, real *c__, real *s, real *work, integer *info);

int slalsa_(integer *icompq, integer *smlsiz, integer *n, integer *nrhs, real *b, integer *ldb, real *bx, integer *ldbx,
    real *u, integer *ldu, real *vt, integer *k, real *difl, real *difr, real *z__, real *poles, integer *givptr,
    integer *givcol, integer *ldgcol, integer *perm, real *givnum, real *c__, real *s, real *work, integer *iwork,
    integer *info);

int slalsd_(char *uplo, integer *smlsiz, integer *n, integer *nrhs, real *d__, real *e, real *b, integer *ldb,
    real *rcond, integer *rank, real *work, integer *iwork, integer *info);

int slamrg_(integer *n1, integer *n2, real *a, integer *strd1, integer *strd2, integer *index);

integer slaneg_(integer *n, real *d__, real *lld, real *sigma, real *pivmin, integer *r__);

doublereal slangb_(char *norm, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, real *work);

doublereal slange_(char *norm, integer *m, integer *n, real *a, integer *lda, real *work);

doublereal slangt_(char *norm, integer *n, real *dl, real *d__, real *du);

doublereal slanhs_(char *norm, integer *n, real *a, integer *lda, real *work);

doublereal slansb_(char *norm, char *uplo, integer *n, integer *k, real *ab, integer *ldab, real *work);

doublereal slansf_(char *norm, char *transr, char *uplo, integer *n, real *a, real *work);

doublereal slansp_(char *norm, char *uplo, integer *n, real *ap, real *work);

doublereal slanst_(char *norm, integer *n, real *d__, real *e);

doublereal slansy_(char *norm, char *uplo, integer *n, real *a, integer *lda, real *work);

doublereal slantb_(char *norm, char *uplo, char *diag, integer *n, integer *k, real *ab, integer *ldab, real *work);

doublereal slantp_(char *norm, char *uplo, char *diag, integer *n, real *ap, real *work);

doublereal slantr_(char *norm, char *uplo, char *diag, integer *m, integer *n, real *a, integer *lda, real *work);

int slanv2_(real *a, real *b, real *c__, real *d__, real *rt1r, real *rt1i, real *rt2r, real *rt2i, real *cs, real *sn);

int slapll_(integer *n, real *x, integer *incx, real *y, integer *incy, real *ssmin);

int slapmt_(logical *forwrd, integer *m, integer *n, real *x, integer *ldx, integer *k);

doublereal slapy2_(real *x, real *y);

doublereal slapy3_(real *x, real *y, real *z__);

int slaqgb_(integer *m, integer *n, integer *kl, integer *ku, real *ab, integer *ldab, real *r__, real *c__,
    real *rowcnd, real *colcnd, real *amax, char *equed);

int slaqge_(integer *m, integer *n, real *a, integer *lda, real *r__, real *c__, real *rowcnd, real *colcnd, real *amax,
    char *equed);

int slaqp2_(integer *m, integer *n, integer *offset, real *a, integer *lda, integer *jpvt, real *tau, real *vn1,
    real *vn2, real *work);

int slaqps_(integer *m, integer *n, integer *offset, integer *nb, integer *kb, real *a, integer *lda, integer *jpvt,
    real *tau, real *vn1, real *vn2, real *auxv, real *f, integer *ldf);

int slaqr0_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, real *h__, integer *ldh, real *wr,
    real *wi, integer *iloz, integer *ihiz, real *z__, integer *ldz, real *work, integer *lwork, integer *info);

int slaqr1_(integer *n, real *h__, integer *ldh, real *sr1, real *si1, real *sr2, real *si2, real *v);

int slaqr2_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, real *h__,
    integer *ldh, integer *iloz, integer *ihiz, real *z__, integer *ldz, integer *ns, integer *nd, real *sr, real *si,
    real *v, integer *ldv, integer *nh, real *t, integer *ldt, integer *nv, real *wv, integer *ldwv, real *work,
    integer *lwork);

int slaqr3_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, real *h__,
    integer *ldh, integer *iloz, integer *ihiz, real *z__, integer *ldz, integer *ns, integer *nd, real *sr, real *si,
    real *v, integer *ldv, integer *nh, real *t, integer *ldt, integer *nv, real *wv, integer *ldwv, real *work,
    integer *lwork);

int slaqr4_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, real *h__, integer *ldh, real *wr,
    real *wi, integer *iloz, integer *ihiz, real *z__, integer *ldz, real *work, integer *lwork, integer *info);

int slaqr5_(logical *wantt, logical *wantz, integer *kacc22, integer *n, integer *ktop, integer *kbot, integer *nshfts,
    real *sr, real *si, real *h__, integer *ldh, integer *iloz, integer *ihiz, real *z__, integer *ldz, real *v,
    integer *ldv, real *u, integer *ldu, integer *nv, real *wv, integer *ldwv, integer *nh, real *wh, integer *ldwh);

int slaqsb_(
    char *uplo, integer *n, integer *kd, real *ab, integer *ldab, real *s, real *scond, real *amax, char *equed);

int slaqsp_(char *uplo, integer *n, real *ap, real *s, real *scond, real *amax, char *equed);

int slaqsy_(char *uplo, integer *n, real *a, integer *lda, real *s, real *scond, real *amax, char *equed);

int slaqtr_(logical *ltran, logical *lreal, integer *n, real *t, integer *ldt, real *b, real *w, real *scale, real *x,
    real *work, integer *info);

int slar1v_(integer *n, integer *b1, integer *bn, real *lambda, real *d__, real *l, real *ld, real *lld, real *pivmin,
    real *gaptol, real *z__, logical *wantnc, integer *negcnt, real *ztz, real *mingma, integer *r__, integer *isuppz,
    real *nrminv, real *resid, real *rqcorr, real *work);

int slar2v_(integer *n, real *x, real *y, real *z__, integer *incx, real *c__, real *s, integer *incc);

int slarf_(char *side, integer *m, integer *n, real *v, integer *incv, real *tau, real *c__, integer *ldc, real *work);

int slarfb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, real *v,
    integer *ldv, real *t, integer *ldt, real *c__, integer *ldc, real *work, integer *ldwork);

int slarfg_(integer *n, real *alpha, real *x, integer *incx, real *tau);

int slarfp_(integer *n, real *alpha, real *x, integer *incx, real *tau);

int slarft_(
    char *direct, char *storev, integer *n, integer *k, real *v, integer *ldv, real *tau, real *t, integer *ldt);

int slarfx_(char *side, integer *m, integer *n, real *v, real *tau, real *c__, integer *ldc, real *work);

int slargv_(integer *n, real *x, integer *incx, real *y, integer *incy, real *c__, integer *incc);

int slarnv_(integer *idist, integer *iseed, integer *n, real *x);

int slarra_(integer *n, real *d__, real *e, real *e2, real *spltol, real *tnrm, integer *nsplit, integer *isplit,
    integer *info);

int slarrb_(integer *n, real *d__, real *lld, integer *ifirst, integer *ilast, real *rtol1, real *rtol2,
    integer *offset, real *w, real *wgap, real *werr, real *work, integer *iwork, real *pivmin, real *spdiam,
    integer *twist, integer *info);

int slarrc_(char *jobt, integer *n, real *vl, real *vu, real *d__, real *e, real *pivmin, integer *eigcnt,
    integer *lcnt, integer *rcnt, integer *info);

int slarrd_(char *range, char *order, integer *n, real *vl, real *vu, integer *il, integer *iu, real *gers,
    real *reltol, real *d__, real *e, real *e2, real *pivmin, integer *nsplit, integer *isplit, integer *m, real *w,
    real *werr, real *wl, real *wu, integer *iblock, integer *indexw, real *work, integer *iwork, integer *info);

int slarre_(char *range, integer *n, real *vl, real *vu, integer *il, integer *iu, real *d__, real *e, real *e2,
    real *rtol1, real *rtol2, real *spltol, integer *nsplit, integer *isplit, integer *m, real *w, real *werr,
    real *wgap, integer *iblock, integer *indexw, real *gers, real *pivmin, real *work, integer *iwork, integer *info);

int slarrf_(integer *n, real *d__, real *l, real *ld, integer *clstrt, integer *clend, real *w, real *wgap, real *werr,
    real *spdiam, real *clgapl, real *clgapr, real *pivmin, real *sigma, real *dplus, real *lplus, real *work,
    integer *info);

int slarrj_(integer *n, real *d__, real *e2, integer *ifirst, integer *ilast, real *rtol, integer *offset, real *w,
    real *werr, real *work, integer *iwork, real *pivmin, real *spdiam, integer *info);

int slarrk_(integer *n, integer *iw, real *gl, real *gu, real *d__, real *e2, real *pivmin, real *reltol, real *w,
    real *werr, integer *info);

int slarrr_(integer *n, real *d__, real *e, integer *info);

int slarrv_(integer *n, real *vl, real *vu, real *d__, real *l, real *pivmin, integer *isplit, integer *m, integer *dol,
    integer *dou, real *minrgp, real *rtol1, real *rtol2, real *w, real *werr, real *wgap, integer *iblock,
    integer *indexw, real *gers, real *z__, integer *ldz, integer *isuppz, real *work, integer *iwork, integer *info);

int slarscl2_(integer *m, integer *n, real *d__, real *x, integer *ldx);

int slartg_(real *f, real *g, real *cs, real *sn, real *r__);

int slartv_(integer *n, real *x, integer *incx, real *y, integer *incy, real *c__, real *s, integer *incc);

int slaruv_(integer *iseed, integer *n, real *x);

int slarz_(char *side, integer *m, integer *n, integer *l, real *v, integer *incv, real *tau, real *c__, integer *ldc,
    real *work);

int slarzb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, integer *l,
    real *v, integer *ldv, real *t, integer *ldt, real *c__, integer *ldc, real *work, integer *ldwork);

int slarzt_(
    char *direct, char *storev, integer *n, integer *k, real *v, integer *ldv, real *tau, real *t, integer *ldt);

int slas2_(real *f, real *g, real *h__, real *ssmin, real *ssmax);

int slascl_(char *type__, integer *kl, integer *ku, real *cfrom, real *cto, integer *m, integer *n, real *a,
    integer *lda, integer *info);

int slascl2_(integer *m, integer *n, real *d__, real *x, integer *ldx);

int slasd0_(integer *n, integer *sqre, real *d__, real *e, real *u, integer *ldu, real *vt, integer *ldvt,
    integer *smlsiz, integer *iwork, real *work, integer *info);

int slasd1_(integer *nl, integer *nr, integer *sqre, real *d__, real *alpha, real *beta, real *u, integer *ldu,
    real *vt, integer *ldvt, integer *idxq, integer *iwork, real *work, integer *info);

int slasd2_(integer *nl, integer *nr, integer *sqre, integer *k, real *d__, real *z__, real *alpha, real *beta, real *u,
    integer *ldu, real *vt, integer *ldvt, real *dsigma, real *u2, integer *ldu2, real *vt2, integer *ldvt2,
    integer *idxp, integer *idx, integer *idxc, integer *idxq, integer *coltyp, integer *info);

int slasd3_(integer *nl, integer *nr, integer *sqre, integer *k, real *d__, real *q, integer *ldq, real *dsigma,
    real *u, integer *ldu, real *u2, integer *ldu2, real *vt, integer *ldvt, real *vt2, integer *ldvt2, integer *idxc,
    integer *ctot, real *z__, integer *info);

int slasd4_(
    integer *n, integer *i__, real *d__, real *z__, real *delta, real *rho, real *sigma, real *work, integer *info);

int slasd5_(integer *i__, real *d__, real *z__, real *delta, real *rho, real *dsigma, real *work);

int slasd6_(integer *icompq, integer *nl, integer *nr, integer *sqre, real *d__, real *vf, real *vl, real *alpha,
    real *beta, integer *idxq, integer *perm, integer *givptr, integer *givcol, integer *ldgcol, real *givnum,
    integer *ldgnum, real *poles, real *difl, real *difr, real *z__, integer *k, real *c__, real *s, real *work,
    integer *iwork, integer *info);

int slasd7_(integer *icompq, integer *nl, integer *nr, integer *sqre, integer *k, real *d__, real *z__, real *zw,
    real *vf, real *vfw, real *vl, real *vlw, real *alpha, real *beta, real *dsigma, integer *idx, integer *idxp,
    integer *idxq, integer *perm, integer *givptr, integer *givcol, integer *ldgcol, real *givnum, integer *ldgnum,
    real *c__, real *s, integer *info);

int slasd8_(integer *icompq, integer *k, real *d__, real *z__, real *vf, real *vl, real *difl, real *difr,
    integer *lddifr, real *dsigma, real *work, integer *info);

int slasda_(integer *icompq, integer *smlsiz, integer *n, integer *sqre, real *d__, real *e, real *u, integer *ldu,
    real *vt, integer *k, real *difl, real *difr, real *z__, real *poles, integer *givptr, integer *givcol,
    integer *ldgcol, integer *perm, real *givnum, real *c__, real *s, real *work, integer *iwork, integer *info);

int slasdq_(char *uplo, integer *sqre, integer *n, integer *ncvt, integer *nru, integer *ncc, real *d__, real *e,
    real *vt, integer *ldvt, real *u, integer *ldu, real *c__, integer *ldc, real *work, integer *info);

int slasdt_(integer *n, integer *lvl, integer *nd, integer *inode, integer *ndiml, integer *ndimr, integer *msub);

int slaset_(char *uplo, integer *m, integer *n, real *alpha, real *beta, real *a, integer *lda);

int slasq1_(integer *n, real *d__, real *e, real *work, integer *info);

int slasq2_(integer *n, real *z__, integer *info);

int slasq3_(integer *i0, integer *n0, real *z__, integer *pp, real *dmin__, real *sigma, real *desig, real *qmax,
    integer *nfail, integer *iter, integer *ndiv, logical *ieee, integer *ttype, real *dmin1, real *dmin2, real *dn,
    real *dn1, real *dn2, real *g, real *tau);

int slasq4_(integer *i0, integer *n0, real *z__, integer *pp, integer *n0in, real *dmin__, real *dmin1, real *dmin2,
    real *dn, real *dn1, real *dn2, real *tau, integer *ttype, real *g);

int slasq5_(integer *i0, integer *n0, real *z__, integer *pp, real *tau, real *dmin__, real *dmin1, real *dmin2,
    real *dn, real *dnm1, real *dnm2, logical *ieee);

int slasq6_(integer *i0, integer *n0, real *z__, integer *pp, real *dmin__, real *dmin1, real *dmin2, real *dn,
    real *dnm1, real *dnm2);

int slasr_(char *side, char *pivot, char *direct, integer *m, integer *n, real *c__, real *s, real *a, integer *lda);

int slasrt_(char *id, integer *n, real *d__, integer *info);

int slassq_(integer *n, real *x, integer *incx, real *scale, real *sumsq);

int slasv2_(real *f, real *g, real *h__, real *ssmin, real *ssmax, real *snr, real *csr, real *snl, real *csl);

int slaswp_(integer *n, real *a, integer *lda, integer *k1, integer *k2, integer *ipiv, integer *incx);

int slasy2_(logical *ltranl, logical *ltranr, integer *isgn, integer *n1, integer *n2, real *tl, integer *ldtl,
    real *tr, integer *ldtr, real *b, integer *ldb, real *scale, real *x, integer *ldx, real *xnorm, integer *info);

int slasyf_(char *uplo, integer *n, integer *nb, integer *kb, real *a, integer *lda, integer *ipiv, real *w,
    integer *ldw, integer *info);

int slatbs_(char *uplo, char *trans, char *diag, char *normin, integer *n, integer *kd, real *ab, integer *ldab,
    real *x, real *scale, real *cnorm, integer *info);

int slatdf_(integer *ijob, integer *n, real *z__, integer *ldz, real *rhs, real *rdsum, real *rdscal, integer *ipiv,
    integer *jpiv);

int slatps_(char *uplo, char *trans, char *diag, char *normin, integer *n, real *ap, real *x, real *scale, real *cnorm,
    integer *info);

int slatrd_(char *uplo, integer *n, integer *nb, real *a, integer *lda, real *e, real *tau, real *w, integer *ldw);

int slatrs_(char *uplo, char *trans, char *diag, char *normin, integer *n, real *a, integer *lda, real *x, real *scale,
    real *cnorm, integer *info);

int slatrz_(integer *m, integer *n, integer *l, real *a, integer *lda, real *tau, real *work);

int slatzm_(char *side, integer *m, integer *n, real *v, integer *incv, real *tau, real *c1, real *c2, integer *ldc,
    real *work);

int slauu2_(char *uplo, integer *n, real *a, integer *lda, integer *info);

int slauum_(char *uplo, integer *n, real *a, integer *lda, integer *info);

int sopgtr_(char *uplo, integer *n, real *ap, real *tau, real *q, integer *ldq, real *work, integer *info);

int sopmtr_(char *side, char *uplo, char *trans, integer *m, integer *n, real *ap, real *tau, real *c__, integer *ldc,
    real *work, integer *info);

int sorg2l_(integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *info);

int sorg2r_(integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *info);

int sorgbr_(char *vect, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work,
    integer *lwork, integer *info);

int sorghr_(integer *n, integer *ilo, integer *ihi, real *a, integer *lda, real *tau, real *work, integer *lwork,
    integer *info);

int sorgl2_(integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *info);

int sorglq_(
    integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sorgql_(
    integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sorgqr_(
    integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sorgr2_(integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *info);

int sorgrq_(
    integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sorgtr_(char *uplo, integer *n, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int sorm2l_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *info);

int sorm2r_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *info);

int sormbr_(char *vect, char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau,
    real *c__, integer *ldc, real *work, integer *lwork, integer *info);

int sormhr_(char *side, char *trans, integer *m, integer *n, integer *ilo, integer *ihi, real *a, integer *lda,
    real *tau, real *c__, integer *ldc, real *work, integer *lwork, integer *info);

int sorml2_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *info);

int sormlq_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *lwork, integer *info);

int sormql_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *lwork, integer *info);

int sormqr_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *lwork, integer *info);

int sormr2_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *info);

int sormr3_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, real *a, integer *lda, real *tau,
    real *c__, integer *ldc, real *work, integer *info);

int sormrq_(char *side, char *trans, integer *m, integer *n, integer *k, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *lwork, integer *info);

int sormrz_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, real *a, integer *lda, real *tau,
    real *c__, integer *ldc, real *work, integer *lwork, integer *info);

int sormtr_(char *side, char *uplo, char *trans, integer *m, integer *n, real *a, integer *lda, real *tau, real *c__,
    integer *ldc, real *work, integer *lwork, integer *info);

int spbcon_(char *uplo, integer *n, integer *kd, real *ab, integer *ldab, real *anorm, real *rcond, real *work,
    integer *iwork, integer *info);

int spbequ_(
    char *uplo, integer *n, integer *kd, real *ab, integer *ldab, real *s, real *scond, real *amax, integer *info);

int spbrfs_(char *uplo, integer *n, integer *kd, integer *nrhs, real *ab, integer *ldab, real *afb, integer *ldafb,
    real *b, integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int spbstf_(char *uplo, integer *n, integer *kd, real *ab, integer *ldab, integer *info);

int spbsv_(
    char *uplo, integer *n, integer *kd, integer *nrhs, real *ab, integer *ldab, real *b, integer *ldb, integer *info);

int spbsvx_(char *fact, char *uplo, integer *n, integer *kd, integer *nrhs, real *ab, integer *ldab, real *afb,
    integer *ldafb, char *equed, real *s, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *ferr,
    real *berr, real *work, integer *iwork, integer *info);

int spbtf2_(char *uplo, integer *n, integer *kd, real *ab, integer *ldab, integer *info);

int spbtrf_(char *uplo, integer *n, integer *kd, real *ab, integer *ldab, integer *info);

int spbtrs_(
    char *uplo, integer *n, integer *kd, integer *nrhs, real *ab, integer *ldab, real *b, integer *ldb, integer *info);

int spftrf_(char *transr, char *uplo, integer *n, real *a, integer *info);

int spftri_(char *transr, char *uplo, integer *n, real *a, integer *info);

int spftrs_(char *transr, char *uplo, integer *n, integer *nrhs, real *a, real *b, integer *ldb, integer *info);

int spocon_(
    char *uplo, integer *n, real *a, integer *lda, real *anorm, real *rcond, real *work, integer *iwork, integer *info);

int spoequ_(integer *n, real *a, integer *lda, real *s, real *scond, real *amax, integer *info);

int spoequb_(integer *n, real *a, integer *lda, real *s, real *scond, real *amax, integer *info);

int sporfs_(char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf, real *b,
    integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int sporfsx_(char *uplo, char *equed, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    real *s, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *berr, integer *n_err_bnds__,
    real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, real *work, integer *iwork,
    integer *info);

int sposv_(char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, integer *info);

int sposvx_(char *fact, char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    char *equed, real *s, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *ferr, real *berr, real *work,
    integer *iwork, integer *info);

int sposvxx_(char *fact, char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    char *equed, real *s, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *rpvgrw, real *berr,
    integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, real *work,
    integer *iwork, integer *info);

int spotf2_(char *uplo, integer *n, real *a, integer *lda, integer *info);

int spotrf_(char *uplo, integer *n, real *a, integer *lda, integer *info);

int spotri_(char *uplo, integer *n, real *a, integer *lda, integer *info);

int spotrs_(char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb, integer *info);

int sppcon_(char *uplo, integer *n, real *ap, real *anorm, real *rcond, real *work, integer *iwork, integer *info);

int sppequ_(char *uplo, integer *n, real *ap, real *s, real *scond, real *amax, integer *info);

int spprfs_(char *uplo, integer *n, integer *nrhs, real *ap, real *afp, real *b, integer *ldb, real *x, integer *ldx,
    real *ferr, real *berr, real *work, integer *iwork, integer *info);

int sppsv_(char *uplo, integer *n, integer *nrhs, real *ap, real *b, integer *ldb, integer *info);

int sppsvx_(char *fact, char *uplo, integer *n, integer *nrhs, real *ap, real *afp, char *equed, real *s, real *b,
    integer *ldb, real *x, integer *ldx, real *rcond, real *ferr, real *berr, real *work, integer *iwork,
    integer *info);

int spptrf_(char *uplo, integer *n, real *ap, integer *info);

int spptri_(char *uplo, integer *n, real *ap, integer *info);

int spptrs_(char *uplo, integer *n, integer *nrhs, real *ap, real *b, integer *ldb, integer *info);

int spstf2_(
    char *uplo, integer *n, real *a, integer *lda, integer *piv, integer *rank, real *tol, real *work, integer *info);

int spstrf_(
    char *uplo, integer *n, real *a, integer *lda, integer *piv, integer *rank, real *tol, real *work, integer *info);

int sptcon_(integer *n, real *d__, real *e, real *anorm, real *rcond, real *work, integer *info);

int spteqr_(char *compz, integer *n, real *d__, real *e, real *z__, integer *ldz, real *work, integer *info);

int sptrfs_(integer *n, integer *nrhs, real *d__, real *e, real *df, real *ef, real *b, integer *ldb, real *x,
    integer *ldx, real *ferr, real *berr, real *work, integer *info);

int sptsv_(integer *n, integer *nrhs, real *d__, real *e, real *b, integer *ldb, integer *info);

int sptsvx_(char *fact, integer *n, integer *nrhs, real *d__, real *e, real *df, real *ef, real *b, integer *ldb,
    real *x, integer *ldx, real *rcond, real *ferr, real *berr, real *work, integer *info);

int spttrf_(integer *n, real *d__, real *e, integer *info);

int spttrs_(integer *n, integer *nrhs, real *d__, real *e, real *b, integer *ldb, integer *info);

int sptts2_(integer *n, integer *nrhs, real *d__, real *e, real *b, integer *ldb);

int srscl_(integer *n, real *sa, real *sx, integer *incx);

int ssbev_(char *jobz, char *uplo, integer *n, integer *kd, real *ab, integer *ldab, real *w, real *z__, integer *ldz,
    real *work, integer *info);

int ssbevd_(char *jobz, char *uplo, integer *n, integer *kd, real *ab, integer *ldab, real *w, real *z__, integer *ldz,
    real *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int ssbevx_(char *jobz, char *range, char *uplo, integer *n, integer *kd, real *ab, integer *ldab, real *q,
    integer *ldq, real *vl, real *vu, integer *il, integer *iu, real *abstol, integer *m, real *w, real *z__,
    integer *ldz, real *work, integer *iwork, integer *ifail, integer *info);

int ssbgst_(char *vect, char *uplo, integer *n, integer *ka, integer *kb, real *ab, integer *ldab, real *bb,
    integer *ldbb, real *x, integer *ldx, real *work, integer *info);

int ssbgv_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, real *ab, integer *ldab, real *bb,
    integer *ldbb, real *w, real *z__, integer *ldz, real *work, integer *info);

int ssbgvd_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, real *ab, integer *ldab, real *bb,
    integer *ldbb, real *w, real *z__, integer *ldz, real *work, integer *lwork, integer *iwork, integer *liwork,
    integer *info);

int ssbgvx_(char *jobz, char *range, char *uplo, integer *n, integer *ka, integer *kb, real *ab, integer *ldab,
    real *bb, integer *ldbb, real *q, integer *ldq, real *vl, real *vu, integer *il, integer *iu, real *abstol,
    integer *m, real *w, real *z__, integer *ldz, real *work, integer *iwork, integer *ifail, integer *info);

int ssbtrd_(char *vect, char *uplo, integer *n, integer *kd, real *ab, integer *ldab, real *d__, real *e, real *q,
    integer *ldq, real *work, integer *info);

int ssfrk_(char *transr, char *uplo, char *trans, integer *n, integer *k, real *alpha, real *a, integer *lda,
    real *beta, real *c__);

int sspcon_(char *uplo, integer *n, real *ap, integer *ipiv, real *anorm, real *rcond, real *work, integer *iwork,
    integer *info);

int sspev_(char *jobz, char *uplo, integer *n, real *ap, real *w, real *z__, integer *ldz, real *work, integer *info);

int sspevd_(char *jobz, char *uplo, integer *n, real *ap, real *w, real *z__, integer *ldz, real *work, integer *lwork,
    integer *iwork, integer *liwork, integer *info);

int sspevx_(char *jobz, char *range, char *uplo, integer *n, real *ap, real *vl, real *vu, integer *il, integer *iu,
    real *abstol, integer *m, real *w, real *z__, integer *ldz, real *work, integer *iwork, integer *ifail,
    integer *info);

int sspgst_(integer *itype, char *uplo, integer *n, real *ap, real *bp, integer *info);

int sspgv_(integer *itype, char *jobz, char *uplo, integer *n, real *ap, real *bp, real *w, real *z__, integer *ldz,
    real *work, integer *info);

int sspgvd_(integer *itype, char *jobz, char *uplo, integer *n, real *ap, real *bp, real *w, real *z__, integer *ldz,
    real *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int sspgvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, real *ap, real *bp, real *vl, real *vu,
    integer *il, integer *iu, real *abstol, integer *m, real *w, real *z__, integer *ldz, real *work, integer *iwork,
    integer *ifail, integer *info);

int ssprfs_(char *uplo, integer *n, integer *nrhs, real *ap, real *afp, integer *ipiv, real *b, integer *ldb, real *x,
    integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int sspsv_(char *uplo, integer *n, integer *nrhs, real *ap, integer *ipiv, real *b, integer *ldb, integer *info);

int sspsvx_(char *fact, char *uplo, integer *n, integer *nrhs, real *ap, real *afp, integer *ipiv, real *b,
    integer *ldb, real *x, integer *ldx, real *rcond, real *ferr, real *berr, real *work, integer *iwork,
    integer *info);

int ssptrd_(char *uplo, integer *n, real *ap, real *d__, real *e, real *tau, integer *info);

int ssptrf_(char *uplo, integer *n, real *ap, integer *ipiv, integer *info);

int ssptri_(char *uplo, integer *n, real *ap, integer *ipiv, real *work, integer *info);

int ssptrs_(char *uplo, integer *n, integer *nrhs, real *ap, integer *ipiv, real *b, integer *ldb, integer *info);

int sstebz_(char *range, char *order, integer *n, real *vl, real *vu, integer *il, integer *iu, real *abstol, real *d__,
    real *e, integer *m, integer *nsplit, real *w, integer *iblock, integer *isplit, real *work, integer *iwork,
    integer *info);

int sstedc_(char *compz, integer *n, real *d__, real *e, real *z__, integer *ldz, real *work, integer *lwork,
    integer *iwork, integer *liwork, integer *info);

int sstegr_(char *jobz, char *range, integer *n, real *d__, real *e, real *vl, real *vu, integer *il, integer *iu,
    real *abstol, integer *m, real *w, real *z__, integer *ldz, integer *isuppz, real *work, integer *lwork,
    integer *iwork, integer *liwork, integer *info);

int sstein_(integer *n, real *d__, real *e, integer *m, real *w, integer *iblock, integer *isplit, real *z__,
    integer *ldz, real *work, integer *iwork, integer *ifail, integer *info);

int sstemr_(char *jobz, char *range, integer *n, real *d__, real *e, real *vl, real *vu, integer *il, integer *iu,
    integer *m, real *w, real *z__, integer *ldz, integer *nzc, integer *isuppz, logical *tryrac, real *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int ssteqr_(char *compz, integer *n, real *d__, real *e, real *z__, integer *ldz, real *work, integer *info);

int ssterf_(integer *n, real *d__, real *e, integer *info);

int sstev_(char *jobz, integer *n, real *d__, real *e, real *z__, integer *ldz, real *work, integer *info);

int sstevd_(char *jobz, integer *n, real *d__, real *e, real *z__, integer *ldz, real *work, integer *lwork,
    integer *iwork, integer *liwork, integer *info);

int sstevr_(char *jobz, char *range, integer *n, real *d__, real *e, real *vl, real *vu, integer *il, integer *iu,
    real *abstol, integer *m, real *w, real *z__, integer *ldz, integer *isuppz, real *work, integer *lwork,
    integer *iwork, integer *liwork, integer *info);

int sstevx_(char *jobz, char *range, integer *n, real *d__, real *e, real *vl, real *vu, integer *il, integer *iu,
    real *abstol, integer *m, real *w, real *z__, integer *ldz, real *work, integer *iwork, integer *ifail,
    integer *info);

int ssycon_(char *uplo, integer *n, real *a, integer *lda, integer *ipiv, real *anorm, real *rcond, real *work,
    integer *iwork, integer *info);

int ssyequb_(
    char *uplo, integer *n, real *a, integer *lda, real *s, real *scond, real *amax, real *work, integer *info);

int ssyev_(
    char *jobz, char *uplo, integer *n, real *a, integer *lda, real *w, real *work, integer *lwork, integer *info);

int ssyevd_(char *jobz, char *uplo, integer *n, real *a, integer *lda, real *w, real *work, integer *lwork,
    integer *iwork, integer *liwork, integer *info);

int ssyevr_(char *jobz, char *range, char *uplo, integer *n, real *a, integer *lda, real *vl, real *vu, integer *il,
    integer *iu, real *abstol, integer *m, real *w, real *z__, integer *ldz, integer *isuppz, real *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int ssyevx_(char *jobz, char *range, char *uplo, integer *n, real *a, integer *lda, real *vl, real *vu, integer *il,
    integer *iu, real *abstol, integer *m, real *w, real *z__, integer *ldz, real *work, integer *lwork, integer *iwork,
    integer *ifail, integer *info);

int ssygs2_(integer *itype, char *uplo, integer *n, real *a, integer *lda, real *b, integer *ldb, integer *info);

int ssygst_(integer *itype, char *uplo, integer *n, real *a, integer *lda, real *b, integer *ldb, integer *info);

int ssygv_(integer *itype, char *jobz, char *uplo, integer *n, real *a, integer *lda, real *b, integer *ldb, real *w,
    real *work, integer *lwork, integer *info);

int ssygvd_(integer *itype, char *jobz, char *uplo, integer *n, real *a, integer *lda, real *b, integer *ldb, real *w,
    real *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int ssygvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, real *a, integer *lda, real *b,
    integer *ldb, real *vl, real *vu, integer *il, integer *iu, real *abstol, integer *m, real *w, real *z__,
    integer *ldz, real *work, integer *lwork, integer *iwork, integer *ifail, integer *info);

int ssyrfs_(char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf, integer *ipiv,
    real *b, integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int ssyrfsx_(char *uplo, char *equed, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    integer *ipiv, real *s, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *berr,
    integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params, real *work,
    integer *iwork, integer *info);

int ssysv_(char *uplo, integer *n, integer *nrhs, real *a, integer *lda, integer *ipiv, real *b, integer *ldb,
    real *work, integer *lwork, integer *info);

int ssysvx_(char *fact, char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    integer *ipiv, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *ferr, real *berr, real *work,
    integer *lwork, integer *iwork, integer *info);

int ssysvxx_(char *fact, char *uplo, integer *n, integer *nrhs, real *a, integer *lda, real *af, integer *ldaf,
    integer *ipiv, char *equed, real *s, real *b, integer *ldb, real *x, integer *ldx, real *rcond, real *rpvgrw,
    real *berr, integer *n_err_bnds__, real *err_bnds_norm__, real *err_bnds_comp__, integer *nparams, real *params,
    real *work, integer *iwork, integer *info);

int ssytd2_(char *uplo, integer *n, real *a, integer *lda, real *d__, real *e, real *tau, integer *info);

int ssytf2_(char *uplo, integer *n, real *a, integer *lda, integer *ipiv, integer *info);

int ssytrd_(char *uplo, integer *n, real *a, integer *lda, real *d__, real *e, real *tau, real *work, integer *lwork,
    integer *info);

int ssytrf_(char *uplo, integer *n, real *a, integer *lda, integer *ipiv, real *work, integer *lwork, integer *info);

int ssytri_(char *uplo, integer *n, real *a, integer *lda, integer *ipiv, real *work, integer *info);

int ssytrs_(
    char *uplo, integer *n, integer *nrhs, real *a, integer *lda, integer *ipiv, real *b, integer *ldb, integer *info);

int stbcon_(char *norm, char *uplo, char *diag, integer *n, integer *kd, real *ab, integer *ldab, real *rcond,
    real *work, integer *iwork, integer *info);

int stbrfs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, real *ab, integer *ldab,
    real *b, integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int stbtrs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, real *ab, integer *ldab,
    real *b, integer *ldb, integer *info);

int stfsm_(char *transr, char *side, char *uplo, char *trans, char *diag, integer *m, integer *n, real *alpha, real *a,
    real *b, integer *ldb);

int stftri_(char *transr, char *uplo, char *diag, integer *n, real *a, integer *info);

int stfttp_(char *transr, char *uplo, integer *n, real *arf, real *ap, integer *info);

int stfttr_(char *transr, char *uplo, integer *n, real *arf, real *a, integer *lda, integer *info);

int stgevc_(char *side, char *howmny, logical *select, integer *n, real *s, integer *lds, real *p, integer *ldp,
    real *vl, integer *ldvl, real *vr, integer *ldvr, integer *mm, integer *m, real *work, integer *info);

int stgex2_(logical *wantq, logical *wantz, integer *n, real *a, integer *lda, real *b, integer *ldb, real *q,
    integer *ldq, real *z__, integer *ldz, integer *j1, integer *n1, integer *n2, real *work, integer *lwork,
    integer *info);

int stgexc_(logical *wantq, logical *wantz, integer *n, real *a, integer *lda, real *b, integer *ldb, real *q,
    integer *ldq, real *z__, integer *ldz, integer *ifst, integer *ilst, real *work, integer *lwork, integer *info);

int stgsen_(integer *ijob, logical *wantq, logical *wantz, logical *select, integer *n, real *a, integer *lda, real *b,
    integer *ldb, real *alphar, real *alphai, real *beta, real *q, integer *ldq, real *z__, integer *ldz, integer *m,
    real *pl, real *pr, real *dif, real *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int stgsja_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, integer *k, integer *l, real *a,
    integer *lda, real *b, integer *ldb, real *tola, real *tolb, real *alpha, real *beta, real *u, integer *ldu,
    real *v, integer *ldv, real *q, integer *ldq, real *work, integer *ncycle, integer *info);

int stgsna_(char *job, char *howmny, logical *select, integer *n, real *a, integer *lda, real *b, integer *ldb,
    real *vl, integer *ldvl, real *vr, integer *ldvr, real *s, real *dif, integer *mm, integer *m, real *work,
    integer *lwork, integer *iwork, integer *info);

int stgsy2_(char *trans, integer *ijob, integer *m, integer *n, real *a, integer *lda, real *b, integer *ldb, real *c__,
    integer *ldc, real *d__, integer *ldd, real *e, integer *lde, real *f, integer *ldf, real *scale, real *rdsum,
    real *rdscal, integer *iwork, integer *pq, integer *info);

int stgsyl_(char *trans, integer *ijob, integer *m, integer *n, real *a, integer *lda, real *b, integer *ldb, real *c__,
    integer *ldc, real *d__, integer *ldd, real *e, integer *lde, real *f, integer *ldf, real *scale, real *dif,
    real *work, integer *lwork, integer *iwork, integer *info);

int stpcon_(
    char *norm, char *uplo, char *diag, integer *n, real *ap, real *rcond, real *work, integer *iwork, integer *info);

int stprfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, real *ap, real *b, integer *ldb, real *x,
    integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int stptri_(char *uplo, char *diag, integer *n, real *ap, integer *info);

int stptrs_(
    char *uplo, char *trans, char *diag, integer *n, integer *nrhs, real *ap, real *b, integer *ldb, integer *info);

int stpttf_(char *transr, char *uplo, integer *n, real *ap, real *arf, integer *info);

int stpttr_(char *uplo, integer *n, real *ap, real *a, integer *lda, integer *info);

int strcon_(char *norm, char *uplo, char *diag, integer *n, real *a, integer *lda, real *rcond, real *work,
    integer *iwork, integer *info);

int strevc_(char *side, char *howmny, logical *select, integer *n, real *t, integer *ldt, real *vl, integer *ldvl,
    real *vr, integer *ldvr, integer *mm, integer *m, real *work, integer *info);

int strexc_(char *compq, integer *n, real *t, integer *ldt, real *q, integer *ldq, integer *ifst, integer *ilst,
    real *work, integer *info);

int strrfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, real *a, integer *lda, real *b,
    integer *ldb, real *x, integer *ldx, real *ferr, real *berr, real *work, integer *iwork, integer *info);

int strsen_(char *job, char *compq, logical *select, integer *n, real *t, integer *ldt, real *q, integer *ldq, real *wr,
    real *wi, integer *m, real *s, real *sep, real *work, integer *lwork, integer *iwork, integer *liwork,
    integer *info);

int strsna_(char *job, char *howmny, logical *select, integer *n, real *t, integer *ldt, real *vl, integer *ldvl,
    real *vr, integer *ldvr, real *s, real *sep, integer *mm, integer *m, real *work, integer *ldwork, integer *iwork,
    integer *info);

int strsyl_(char *trana, char *tranb, integer *isgn, integer *m, integer *n, real *a, integer *lda, real *b,
    integer *ldb, real *c__, integer *ldc, real *scale, integer *info);

int strti2_(char *uplo, char *diag, integer *n, real *a, integer *lda, integer *info);

int strtri_(char *uplo, char *diag, integer *n, real *a, integer *lda, integer *info);

int strtrs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, real *a, integer *lda, real *b,
    integer *ldb, integer *info);

int strttf_(char *transr, char *uplo, integer *n, real *a, integer *lda, real *arf, integer *info);

int strttp_(char *uplo, integer *n, real *a, integer *lda, real *ap, integer *info);

int stzrqf_(integer *m, integer *n, real *a, integer *lda, real *tau, integer *info);

int stzrzf_(integer *m, integer *n, real *a, integer *lda, real *tau, real *work, integer *lwork, integer *info);

int xerbla_(char *srname, integer *info);

int xerbla_array__(char *srname_array__, integer *srname_len__, integer *info, ftnlen srname_array_len);

int zbdsqr_(char *uplo, integer *n, integer *ncvt, integer *nru, integer *ncc, doublereal *d__, doublereal *e,
    doublecomplex *vt, integer *ldvt, doublecomplex *u, integer *ldu, doublecomplex *c__, integer *ldc,
    doublereal *rwork, integer *info);

int zcgesv_(integer *n, integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *b, integer *ldb,
    doublecomplex *x, integer *ldx, doublecomplex *work, complex *swork, doublereal *rwork, integer *iter,
    integer *info);

int zcposv_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *x, integer *ldx, doublecomplex *work, complex *swork, doublereal *rwork, integer *iter,
    integer *info);

int zdrscl_(integer *n, doublereal *sa, doublecomplex *sx, integer *incx);

int zgbbrd_(char *vect, integer *m, integer *n, integer *ncc, integer *kl, integer *ku, doublecomplex *ab,
    integer *ldab, doublereal *d__, doublereal *e, doublecomplex *q, integer *ldq, doublecomplex *pt, integer *ldpt,
    doublecomplex *c__, integer *ldc, doublecomplex *work, doublereal *rwork, integer *info);

int zgbcon_(char *norm, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab, integer *ipiv,
    doublereal *anorm, doublereal *rcond, doublecomplex *work, doublereal *rwork, integer *info);

int zgbequ_(integer *m, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab, doublereal *r__,
    doublereal *c__, doublereal *rowcnd, doublereal *colcnd, doublereal *amax, integer *info);

int zgbequb_(integer *m, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab, doublereal *r__,
    doublereal *c__, doublereal *rowcnd, doublereal *colcnd, doublereal *amax, integer *info);

int zgbrfs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublecomplex *ab, integer *ldab,
    doublecomplex *afb, integer *ldafb, integer *ipiv, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx,
    doublereal *ferr, doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zgbrfsx_(char *trans, char *equed, integer *n, integer *kl, integer *ku, integer *nrhs, doublecomplex *ab,
    integer *ldab, doublecomplex *afb, integer *ldafb, integer *ipiv, doublereal *r__, doublereal *c__,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *berr,
    integer *n_err_bnds__, doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams,
    doublereal *params, doublecomplex *work, doublereal *rwork, integer *info);

int zgbsv_(integer *n, integer *kl, integer *ku, integer *nrhs, doublecomplex *ab, integer *ldab, integer *ipiv,
    doublecomplex *b, integer *ldb, integer *info);

int zgbsvx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublecomplex *ab,
    integer *ldab, doublecomplex *afb, integer *ldafb, integer *ipiv, char *equed, doublereal *r__, doublereal *c__,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *ferr,
    doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zgbsvxx_(char *fact, char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublecomplex *ab,
    integer *ldab, doublecomplex *afb, integer *ldafb, integer *ipiv, char *equed, doublereal *r__, doublereal *c__,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *rpvgrw,
    doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams,
    doublereal *params, doublecomplex *work, doublereal *rwork, integer *info);

int zgbtf2_(
    integer *m, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab, integer *ipiv, integer *info);

int zgbtrf_(
    integer *m, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab, integer *ipiv, integer *info);

int zgbtrs_(char *trans, integer *n, integer *kl, integer *ku, integer *nrhs, doublecomplex *ab, integer *ldab,
    integer *ipiv, doublecomplex *b, integer *ldb, integer *info);

int zgebak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, doublereal *scale, integer *m,
    doublecomplex *v, integer *ldv, integer *info);

int zgebal_(char *job, integer *n, doublecomplex *a, integer *lda, integer *ilo, integer *ihi, doublereal *scale,
    integer *info);

int zgebd2_(integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *d__, doublereal *e, doublecomplex *tauq,
    doublecomplex *taup, doublecomplex *work, integer *info);

int zgebrd_(integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *d__, doublereal *e, doublecomplex *tauq,
    doublecomplex *taup, doublecomplex *work, integer *lwork, integer *info);

int zgecon_(char *norm, integer *n, doublecomplex *a, integer *lda, doublereal *anorm, doublereal *rcond,
    doublecomplex *work, doublereal *rwork, integer *info);

int zgeequ_(integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *r__, doublereal *c__,
    doublereal *rowcnd, doublereal *colcnd, doublereal *amax, integer *info);

int zgeequb_(integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *r__, doublereal *c__,
    doublereal *rowcnd, doublereal *colcnd, doublereal *amax, integer *info);

int zgees_(char *jobvs, char *sort, L_fp select, integer *n, doublecomplex *a, integer *lda, integer *sdim,
    doublecomplex *w, doublecomplex *vs, integer *ldvs, doublecomplex *work, integer *lwork, doublereal *rwork,
    logical *bwork, integer *info);

int zgeesx_(char *jobvs, char *sort, L_fp select, char *sense, integer *n, doublecomplex *a, integer *lda,
    integer *sdim, doublecomplex *w, doublecomplex *vs, integer *ldvs, doublereal *rconde, doublereal *rcondv,
    doublecomplex *work, integer *lwork, doublereal *rwork, logical *bwork, integer *info);

int zgeev_(char *jobvl, char *jobvr, integer *n, doublecomplex *a, integer *lda, doublecomplex *w, doublecomplex *vl,
    integer *ldvl, doublecomplex *vr, integer *ldvr, doublecomplex *work, integer *lwork, doublereal *rwork,
    integer *info);

int zgeevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *w, doublecomplex *vl, integer *ldvl, doublecomplex *vr, integer *ldvr, integer *ilo, integer *ihi,
    doublereal *scale, doublereal *abnrm, doublereal *rconde, doublereal *rcondv, doublecomplex *work, integer *lwork,
    doublereal *rwork, integer *info);

int zgegs_(char *jobvsl, char *jobvsr, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *alpha, doublecomplex *beta, doublecomplex *vsl, integer *ldvsl, doublecomplex *vsr, integer *ldvsr,
    doublecomplex *work, integer *lwork, doublereal *rwork, integer *info);

int zgegv_(char *jobvl, char *jobvr, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *alpha, doublecomplex *beta, doublecomplex *vl, integer *ldvl, doublecomplex *vr, integer *ldvr,
    doublecomplex *work, integer *lwork, doublereal *rwork, integer *info);

int zgehd2_(integer *n, integer *ilo, integer *ihi, doublecomplex *a, integer *lda, doublecomplex *tau,
    doublecomplex *work, integer *info);

int zgehrd_(integer *n, integer *ilo, integer *ihi, doublecomplex *a, integer *lda, doublecomplex *tau,
    doublecomplex *work, integer *lwork, integer *info);

int zgelq2_(
    integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work, integer *info);

int zgelqf_(integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zgels_(char *trans, integer *m, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b,
    integer *ldb, doublecomplex *work, integer *lwork, integer *info);

int zgelsd_(integer *m, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublereal *s, doublereal *rcond, integer *rank, doublecomplex *work, integer *lwork, doublereal *rwork,
    integer *iwork, integer *info);

int zgelss_(integer *m, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublereal *s, doublereal *rcond, integer *rank, doublecomplex *work, integer *lwork, doublereal *rwork,
    integer *info);

int zgelsx_(integer *m, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    integer *jpvt, doublereal *rcond, integer *rank, doublecomplex *work, doublereal *rwork, integer *info);

int zgelsy_(integer *m, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    integer *jpvt, doublereal *rcond, integer *rank, doublecomplex *work, integer *lwork, doublereal *rwork,
    integer *info);

int zgeql2_(
    integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work, integer *info);

int zgeqlf_(integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zgeqp3_(integer *m, integer *n, doublecomplex *a, integer *lda, integer *jpvt, doublecomplex *tau,
    doublecomplex *work, integer *lwork, doublereal *rwork, integer *info);

int zgeqpf_(integer *m, integer *n, doublecomplex *a, integer *lda, integer *jpvt, doublecomplex *tau,
    doublecomplex *work, doublereal *rwork, integer *info);

int zgeqr2_(
    integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work, integer *info);

int zgeqrf_(integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zgerfs_(char *trans, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zgerfsx_(char *trans, char *equed, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, doublereal *r__, doublereal *c__, doublecomplex *b, integer *ldb, doublecomplex *x,
    integer *ldx, doublereal *rcond, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublecomplex *work, doublereal *rwork,
    integer *info);

int zgerq2_(
    integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work, integer *info);

int zgerqf_(integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zgesc2_(
    integer *n, doublecomplex *a, integer *lda, doublecomplex *rhs, integer *ipiv, integer *jpiv, doublereal *scale);

int zgesdd_(char *jobz, integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *s, doublecomplex *u,
    integer *ldu, doublecomplex *vt, integer *ldvt, doublecomplex *work, integer *lwork, doublereal *rwork,
    integer *iwork, integer *info);

int zgesv_(integer *n, integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *b, integer *ldb,
    integer *info);

int zgesvd_(char *jobu, char *jobvt, integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *s,
    doublecomplex *u, integer *ldu, doublecomplex *vt, integer *ldvt, doublecomplex *work, integer *lwork,
    doublereal *rwork, integer *info);

int zgesvx_(char *fact, char *trans, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, char *equed, doublereal *r__, doublereal *c__, doublecomplex *b, integer *ldb,
    doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr, doublecomplex *work,
    doublereal *rwork, integer *info);

int zgesvxx_(char *fact, char *trans, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, char *equed, doublereal *r__, doublereal *c__, doublecomplex *b, integer *ldb,
    doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *rpvgrw, doublereal *berr, integer *n_err_bnds__,
    doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublecomplex *work,
    doublereal *rwork, integer *info);

int zgetc2_(integer *n, doublecomplex *a, integer *lda, integer *ipiv, integer *jpiv, integer *info);

int zgetf2_(integer *m, integer *n, doublecomplex *a, integer *lda, integer *ipiv, integer *info);

int zgetrf_(integer *m, integer *n, doublecomplex *a, integer *lda, integer *ipiv, integer *info);

int zgetri_(
    integer *n, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *work, integer *lwork, integer *info);

int zgetrs_(char *trans, integer *n, integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *b,
    integer *ldb, integer *info);

int zggbak_(char *job, char *side, integer *n, integer *ilo, integer *ihi, doublereal *lscale, doublereal *rscale,
    integer *m, doublecomplex *v, integer *ldv, integer *info);

int zggbal_(char *job, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb, integer *ilo,
    integer *ihi, doublereal *lscale, doublereal *rscale, doublereal *work, integer *info);

int zgges_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, integer *sdim, doublecomplex *alpha, doublecomplex *beta, doublecomplex *vsl,
    integer *ldvsl, doublecomplex *vsr, integer *ldvsr, doublecomplex *work, integer *lwork, doublereal *rwork,
    logical *bwork, integer *info);

int zggesx_(char *jobvsl, char *jobvsr, char *sort, L_fp selctg, char *sense, integer *n, doublecomplex *a,
    integer *lda, doublecomplex *b, integer *ldb, integer *sdim, doublecomplex *alpha, doublecomplex *beta,
    doublecomplex *vsl, integer *ldvsl, doublecomplex *vsr, integer *ldvsr, doublereal *rconde, doublereal *rcondv,
    doublecomplex *work, integer *lwork, doublereal *rwork, integer *iwork, integer *liwork, logical *bwork,
    integer *info);

int zggev_(char *jobvl, char *jobvr, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *alpha, doublecomplex *beta, doublecomplex *vl, integer *ldvl, doublecomplex *vr, integer *ldvr,
    doublecomplex *work, integer *lwork, doublereal *rwork, integer *info);

int zggevx_(char *balanc, char *jobvl, char *jobvr, char *sense, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *alpha, doublecomplex *beta, doublecomplex *vl, integer *ldvl,
    doublecomplex *vr, integer *ldvr, integer *ilo, integer *ihi, doublereal *lscale, doublereal *rscale,
    doublereal *abnrm, doublereal *bbnrm, doublereal *rconde, doublereal *rcondv, doublecomplex *work, integer *lwork,
    doublereal *rwork, integer *iwork, logical *bwork, integer *info);

int zggglm_(integer *n, integer *m, integer *p, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *d__, doublecomplex *x, doublecomplex *y, doublecomplex *work, integer *lwork, integer *info);

int zgghrd_(char *compq, char *compz, integer *n, integer *ilo, integer *ihi, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *q, integer *ldq, doublecomplex *z__, integer *ldz, integer *info);

int zgglse_(integer *m, integer *n, integer *p, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *c__, doublecomplex *d__, doublecomplex *x, doublecomplex *work, integer *lwork, integer *info);

int zggqrf_(integer *n, integer *m, integer *p, doublecomplex *a, integer *lda, doublecomplex *taua, doublecomplex *b,
    integer *ldb, doublecomplex *taub, doublecomplex *work, integer *lwork, integer *info);

int zggrqf_(integer *m, integer *p, integer *n, doublecomplex *a, integer *lda, doublecomplex *taua, doublecomplex *b,
    integer *ldb, doublecomplex *taub, doublecomplex *work, integer *lwork, integer *info);

int zggsvd_(char *jobu, char *jobv, char *jobq, integer *m, integer *n, integer *p, integer *k, integer *l,
    doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb, doublereal *alpha, doublereal *beta,
    doublecomplex *u, integer *ldu, doublecomplex *v, integer *ldv, doublecomplex *q, integer *ldq, doublecomplex *work,
    doublereal *rwork, integer *iwork, integer *info);

int zggsvp_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublereal *tola, doublereal *tolb, integer *k, integer *l, doublecomplex *u,
    integer *ldu, doublecomplex *v, integer *ldv, doublecomplex *q, integer *ldq, integer *iwork, doublereal *rwork,
    doublecomplex *tau, doublecomplex *work, integer *info);

int zgtcon_(char *norm, integer *n, doublecomplex *dl, doublecomplex *d__, doublecomplex *du, doublecomplex *du2,
    integer *ipiv, doublereal *anorm, doublereal *rcond, doublecomplex *work, integer *info);

int zgtrfs_(char *trans, integer *n, integer *nrhs, doublecomplex *dl, doublecomplex *d__, doublecomplex *du,
    doublecomplex *dlf, doublecomplex *df, doublecomplex *duf, doublecomplex *du2, integer *ipiv, doublecomplex *b,
    integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr, doublecomplex *work,
    doublereal *rwork, integer *info);

int zgtsv_(integer *n, integer *nrhs, doublecomplex *dl, doublecomplex *d__, doublecomplex *du, doublecomplex *b,
    integer *ldb, integer *info);

int zgtsvx_(char *fact, char *trans, integer *n, integer *nrhs, doublecomplex *dl, doublecomplex *d__,
    doublecomplex *du, doublecomplex *dlf, doublecomplex *df, doublecomplex *duf, doublecomplex *du2, integer *ipiv,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *ferr,
    doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zgttrf_(integer *n, doublecomplex *dl, doublecomplex *d__, doublecomplex *du, doublecomplex *du2, integer *ipiv,
    integer *info);

int zgttrs_(char *trans, integer *n, integer *nrhs, doublecomplex *dl, doublecomplex *d__, doublecomplex *du,
    doublecomplex *du2, integer *ipiv, doublecomplex *b, integer *ldb, integer *info);

int zgtts2_(integer *itrans, integer *n, integer *nrhs, doublecomplex *dl, doublecomplex *d__, doublecomplex *du,
    doublecomplex *du2, integer *ipiv, doublecomplex *b, integer *ldb);

int zhbev_(char *jobz, char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, doublereal *w,
    doublecomplex *z__, integer *ldz, doublecomplex *work, doublereal *rwork, integer *info);

int zhbevd_(char *jobz, char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, doublereal *w,
    doublecomplex *z__, integer *ldz, doublecomplex *work, integer *lwork, doublereal *rwork, integer *lrwork,
    integer *iwork, integer *liwork, integer *info);

int zhbevx_(char *jobz, char *range, char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab,
    doublecomplex *q, integer *ldq, doublereal *vl, doublereal *vu, integer *il, integer *iu, doublereal *abstol,
    integer *m, doublereal *w, doublecomplex *z__, integer *ldz, doublecomplex *work, doublereal *rwork, integer *iwork,
    integer *ifail, integer *info);

int zhbgst_(char *vect, char *uplo, integer *n, integer *ka, integer *kb, doublecomplex *ab, integer *ldab,
    doublecomplex *bb, integer *ldbb, doublecomplex *x, integer *ldx, doublecomplex *work, doublereal *rwork,
    integer *info);

int zhbgv_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, doublecomplex *ab, integer *ldab,
    doublecomplex *bb, integer *ldbb, doublereal *w, doublecomplex *z__, integer *ldz, doublecomplex *work,
    doublereal *rwork, integer *info);

int zhbgvd_(char *jobz, char *uplo, integer *n, integer *ka, integer *kb, doublecomplex *ab, integer *ldab,
    doublecomplex *bb, integer *ldbb, doublereal *w, doublecomplex *z__, integer *ldz, doublecomplex *work,
    integer *lwork, doublereal *rwork, integer *lrwork, integer *iwork, integer *liwork, integer *info);

int zhbgvx_(char *jobz, char *range, char *uplo, integer *n, integer *ka, integer *kb, doublecomplex *ab, integer *ldab,
    doublecomplex *bb, integer *ldbb, doublecomplex *q, integer *ldq, doublereal *vl, doublereal *vu, integer *il,
    integer *iu, doublereal *abstol, integer *m, doublereal *w, doublecomplex *z__, integer *ldz, doublecomplex *work,
    doublereal *rwork, integer *iwork, integer *ifail, integer *info);

int zhbtrd_(char *vect, char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, doublereal *d__,
    doublereal *e, doublecomplex *q, integer *ldq, doublecomplex *work, integer *info);

int zhecon_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, doublereal *anorm, doublereal *rcond,
    doublecomplex *work, integer *info);

int zheequb_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax,
    doublecomplex *work, integer *info);

int zheev_(char *jobz, char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *w, doublecomplex *work,
    integer *lwork, doublereal *rwork, integer *info);

int zheevd_(char *jobz, char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *w, doublecomplex *work,
    integer *lwork, doublereal *rwork, integer *lrwork, integer *iwork, integer *liwork, integer *info);

int zheevr_(char *jobz, char *range, char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *vl,
    doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublecomplex *z__,
    integer *ldz, integer *isuppz, doublecomplex *work, integer *lwork, doublereal *rwork, integer *lrwork,
    integer *iwork, integer *liwork, integer *info);

int zheevx_(char *jobz, char *range, char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *vl,
    doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublecomplex *z__,
    integer *ldz, doublecomplex *work, integer *lwork, doublereal *rwork, integer *iwork, integer *ifail,
    integer *info);

int zhegs2_(integer *itype, char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    integer *info);

int zhegst_(integer *itype, char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    integer *info);

int zhegv_(integer *itype, char *jobz, char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *b,
    integer *ldb, doublereal *w, doublecomplex *work, integer *lwork, doublereal *rwork, integer *info);

int zhegvd_(integer *itype, char *jobz, char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *b,
    integer *ldb, doublereal *w, doublecomplex *work, integer *lwork, doublereal *rwork, integer *lrwork,
    integer *iwork, integer *liwork, integer *info);

int zhegvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublereal *vl, doublereal *vu, integer *il, integer *iu, doublereal *abstol,
    integer *m, doublereal *w, doublecomplex *z__, integer *ldz, doublecomplex *work, integer *lwork, doublereal *rwork,
    integer *iwork, integer *ifail, integer *info);

int zherfs_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zherfsx_(char *uplo, char *equed, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx,
    doublereal *rcond, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublecomplex *work, doublereal *rwork,
    integer *info);

int zhesv_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *b,
    integer *ldb, doublecomplex *work, integer *lwork, integer *info);

int zhesvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond,
    doublereal *ferr, doublereal *berr, doublecomplex *work, integer *lwork, doublereal *rwork, integer *info);

int zhesvxx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, char *equed, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x,
    integer *ldx, doublereal *rcond, doublereal *rpvgrw, doublereal *berr, integer *n_err_bnds__,
    doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublecomplex *work,
    doublereal *rwork, integer *info);

int zhetd2_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *d__, doublereal *e, doublecomplex *tau,
    integer *info);

int zhetf2_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, integer *info);

int zhetrd_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *d__, doublereal *e, doublecomplex *tau,
    doublecomplex *work, integer *lwork, integer *info);

int zhetrf_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *work, integer *lwork,
    integer *info);

int zhetri_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *work, integer *info);

int zhetrs_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *b,
    integer *ldb, integer *info);

int zhfrk_(char *transr, char *uplo, char *trans, integer *n, integer *k, doublereal *alpha, doublecomplex *a,
    integer *lda, doublereal *beta, doublecomplex *c__);

int zhgeqz_(char *job, char *compq, char *compz, integer *n, integer *ilo, integer *ihi, doublecomplex *h__,
    integer *ldh, doublecomplex *t, integer *ldt, doublecomplex *alpha, doublecomplex *beta, doublecomplex *q,
    integer *ldq, doublecomplex *z__, integer *ldz, doublecomplex *work, integer *lwork, doublereal *rwork,
    integer *info);

int zhpcon_(char *uplo, integer *n, doublecomplex *ap, integer *ipiv, doublereal *anorm, doublereal *rcond,
    doublecomplex *work, integer *info);

int zhpev_(char *jobz, char *uplo, integer *n, doublecomplex *ap, doublereal *w, doublecomplex *z__, integer *ldz,
    doublecomplex *work, doublereal *rwork, integer *info);

int zhpevd_(char *jobz, char *uplo, integer *n, doublecomplex *ap, doublereal *w, doublecomplex *z__, integer *ldz,
    doublecomplex *work, integer *lwork, doublereal *rwork, integer *lrwork, integer *iwork, integer *liwork,
    integer *info);

int zhpevx_(char *jobz, char *range, char *uplo, integer *n, doublecomplex *ap, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublecomplex *z__, integer *ldz,
    doublecomplex *work, doublereal *rwork, integer *iwork, integer *ifail, integer *info);

int zhpgst_(integer *itype, char *uplo, integer *n, doublecomplex *ap, doublecomplex *bp, integer *info);

int zhpgv_(integer *itype, char *jobz, char *uplo, integer *n, doublecomplex *ap, doublecomplex *bp, doublereal *w,
    doublecomplex *z__, integer *ldz, doublecomplex *work, doublereal *rwork, integer *info);

int zhpgvd_(integer *itype, char *jobz, char *uplo, integer *n, doublecomplex *ap, doublecomplex *bp, doublereal *w,
    doublecomplex *z__, integer *ldz, doublecomplex *work, integer *lwork, doublereal *rwork, integer *lrwork,
    integer *iwork, integer *liwork, integer *info);

int zhpgvx_(integer *itype, char *jobz, char *range, char *uplo, integer *n, doublecomplex *ap, doublecomplex *bp,
    doublereal *vl, doublereal *vu, integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w,
    doublecomplex *z__, integer *ldz, doublecomplex *work, doublereal *rwork, integer *iwork, integer *ifail,
    integer *info);

int zhprfs_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *afp, integer *ipiv,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zhpsv_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, integer *ipiv, doublecomplex *b, integer *ldb,
    integer *info);

int zhpsvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *afp, integer *ipiv,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *ferr,
    doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zhptrd_(
    char *uplo, integer *n, doublecomplex *ap, doublereal *d__, doublereal *e, doublecomplex *tau, integer *info);

int zhptrf_(char *uplo, integer *n, doublecomplex *ap, integer *ipiv, integer *info);

int zhptri_(char *uplo, integer *n, doublecomplex *ap, integer *ipiv, doublecomplex *work, integer *info);

int zhptrs_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, integer *ipiv, doublecomplex *b, integer *ldb,
    integer *info);

int zhsein_(char *side, char *eigsrc, char *initv, logical *select, integer *n, doublecomplex *h__, integer *ldh,
    doublecomplex *w, doublecomplex *vl, integer *ldvl, doublecomplex *vr, integer *ldvr, integer *mm, integer *m,
    doublecomplex *work, doublereal *rwork, integer *ifaill, integer *ifailr, integer *info);

int zhseqr_(char *job, char *compz, integer *n, integer *ilo, integer *ihi, doublecomplex *h__, integer *ldh,
    doublecomplex *w, doublecomplex *z__, integer *ldz, doublecomplex *work, integer *lwork, integer *info);

int zla_gbamv__(integer *trans, integer *m, integer *n, integer *kl, integer *ku, doublereal *alpha, doublecomplex *ab,
    integer *ldab, doublecomplex *x, integer *incx, doublereal *beta, doublereal *y, integer *incy);

doublereal zla_gbrcond_c__(char *trans, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab,
    doublecomplex *afb, integer *ldafb, integer *ipiv, doublereal *c__, logical *capply, integer *info,
    doublecomplex *work, doublereal *rwork, ftnlen trans_len);

doublereal zla_gbrcond_x__(char *trans, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab,
    doublecomplex *afb, integer *ldafb, integer *ipiv, doublecomplex *x, integer *info, doublecomplex *work,
    doublereal *rwork, ftnlen trans_len);

int zla_gbrfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *kl, integer *ku,
    integer *nrhs, doublecomplex *ab, integer *ldab, doublecomplex *afb, integer *ldafb, integer *ipiv, logical *colequ,
    doublereal *c__, doublecomplex *b, integer *ldb, doublecomplex *y, integer *ldy, doublereal *berr_out__,
    integer *n_norms__, doublereal *errs_n__, doublereal *errs_c__, doublecomplex *res, doublereal *ayb,
    doublecomplex *dy, doublecomplex *y_tail__, doublereal *rcond, integer *ithresh, doublereal *rthresh,
    doublereal *dz_ub__, logical *ignore_cwise__, integer *info);

doublereal zla_gbrpvgrw__(integer *n, integer *kl, integer *ku, integer *ncols, doublecomplex *ab, integer *ldab,
    doublecomplex *afb, integer *ldafb);

int zla_geamv__(integer *trans, integer *m, integer *n, doublereal *alpha, doublecomplex *a, integer *lda,
    doublecomplex *x, integer *incx, doublereal *beta, doublereal *y, integer *incy);

doublereal zla_gercond_c__(char *trans, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublereal *c__, logical *capply, integer *info, doublecomplex *work, doublereal *rwork,
    ftnlen trans_len);

doublereal zla_gercond_x__(char *trans, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublecomplex *x, integer *info, doublecomplex *work, doublereal *rwork, ftnlen trans_len);

int zla_gerfsx_extended__(integer *prec_type__, integer *trans_type__, integer *n, integer *nrhs, doublecomplex *a,
    integer *lda, doublecomplex *af, integer *ldaf, integer *ipiv, logical *colequ, doublereal *c__, doublecomplex *b,
    integer *ldb, doublecomplex *y, integer *ldy, doublereal *berr_out__, integer *n_norms__, doublereal *errs_n__,
    doublereal *errs_c__, doublecomplex *res, doublereal *ayb, doublecomplex *dy, doublecomplex *y_tail__,
    doublereal *rcond, integer *ithresh, doublereal *rthresh, doublereal *dz_ub__, logical *ignore_cwise__,
    integer *info);

int zla_heamv__(integer *uplo, integer *n, doublereal *alpha, doublecomplex *a, integer *lda, doublecomplex *x,
    integer *incx, doublereal *beta, doublereal *y, integer *incy);

doublereal zla_hercond_c__(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublereal *c__, logical *capply, integer *info, doublecomplex *work, doublereal *rwork,
    ftnlen uplo_len);

doublereal zla_hercond_x__(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublecomplex *x, integer *info, doublecomplex *work, doublereal *rwork, ftnlen uplo_len);

int zla_herfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda,
    doublecomplex *af, integer *ldaf, integer *ipiv, logical *colequ, doublereal *c__, doublecomplex *b, integer *ldb,
    doublecomplex *y, integer *ldy, doublereal *berr_out__, integer *n_norms__, doublereal *errs_n__,
    doublereal *errs_c__, doublecomplex *res, doublereal *ayb, doublecomplex *dy, doublecomplex *y_tail__,
    doublereal *rcond, integer *ithresh, doublereal *rthresh, doublereal *dz_ub__, logical *ignore_cwise__,
    integer *info, ftnlen uplo_len);

doublereal zla_herpvgrw__(char *uplo, integer *n, integer *info, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, doublereal *work, ftnlen uplo_len);

int zla_lin_berr__(integer *n, integer *nz, integer *nrhs, doublecomplex *res, doublereal *ayb, doublereal *berr);

doublereal zla_porcond_c__(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    doublereal *c__, logical *capply, integer *info, doublecomplex *work, doublereal *rwork, ftnlen uplo_len);

doublereal zla_porcond_x__(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    doublecomplex *x, integer *info, doublecomplex *work, doublereal *rwork, ftnlen uplo_len);

int zla_porfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda,
    doublecomplex *af, integer *ldaf, logical *colequ, doublereal *c__, doublecomplex *b, integer *ldb,
    doublecomplex *y, integer *ldy, doublereal *berr_out__, integer *n_norms__, doublereal *errs_n__,
    doublereal *errs_c__, doublecomplex *res, doublereal *ayb, doublecomplex *dy, doublecomplex *y_tail__,
    doublereal *rcond, integer *ithresh, doublereal *rthresh, doublereal *dz_ub__, logical *ignore_cwise__,
    integer *info, ftnlen uplo_len);

doublereal zla_porpvgrw__(char *uplo, integer *ncols, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    doublereal *work, ftnlen uplo_len);

doublereal zla_rpvgrw__(integer *n, integer *ncols, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf);

int zla_syamv__(integer *uplo, integer *n, doublereal *alpha, doublecomplex *a, integer *lda, doublecomplex *x,
    integer *incx, doublereal *beta, doublereal *y, integer *incy);

doublereal zla_syrcond_c__(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublereal *c__, logical *capply, integer *info, doublecomplex *work, doublereal *rwork,
    ftnlen uplo_len);

doublereal zla_syrcond_x__(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublecomplex *x, integer *info, doublecomplex *work, doublereal *rwork, ftnlen uplo_len);

int zla_syrfsx_extended__(integer *prec_type__, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda,
    doublecomplex *af, integer *ldaf, integer *ipiv, logical *colequ, doublereal *c__, doublecomplex *b, integer *ldb,
    doublecomplex *y, integer *ldy, doublereal *berr_out__, integer *n_norms__, doublereal *errs_n__,
    doublereal *errs_c__, doublecomplex *res, doublereal *ayb, doublecomplex *dy, doublecomplex *y_tail__,
    doublereal *rcond, integer *ithresh, doublereal *rthresh, doublereal *dz_ub__, logical *ignore_cwise__,
    integer *info, ftnlen uplo_len);

doublereal zla_syrpvgrw__(char *uplo, integer *n, integer *info, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, doublereal *work, ftnlen uplo_len);

int zla_wwaddw__(integer *n, doublecomplex *x, doublecomplex *y, doublecomplex *w);

int zlabrd_(integer *m, integer *n, integer *nb, doublecomplex *a, integer *lda, doublereal *d__, doublereal *e,
    doublecomplex *tauq, doublecomplex *taup, doublecomplex *x, integer *ldx, doublecomplex *y, integer *ldy);

int zlacgv_(integer *n, doublecomplex *x, integer *incx);

int zlacn2_(integer *n, doublecomplex *v, doublecomplex *x, doublereal *est, integer *kase, integer *isave);

int zlacon_(integer *n, doublecomplex *v, doublecomplex *x, doublereal *est, integer *kase);

int zlacp2_(char *uplo, integer *m, integer *n, doublereal *a, integer *lda, doublecomplex *b, integer *ldb);

int zlacpy_(char *uplo, integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb);

int zlacrm_(integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *b, integer *ldb, doublecomplex *c__,
    integer *ldc, doublereal *rwork);

int zlacrt_(integer *n, doublecomplex *cx, integer *incx, doublecomplex *cy, integer *incy, doublecomplex *c__,
    doublecomplex *s);

VOID zladiv_(doublecomplex *ret_val, doublecomplex *x, doublecomplex *y);

int zlaed0_(integer *qsiz, integer *n, doublereal *d__, doublereal *e, doublecomplex *q, integer *ldq,
    doublecomplex *qstore, integer *ldqs, doublereal *rwork, integer *iwork, integer *info);

int zlaed7_(integer *n, integer *cutpnt, integer *qsiz, integer *tlvls, integer *curlvl, integer *curpbm,
    doublereal *d__, doublecomplex *q, integer *ldq, doublereal *rho, integer *indxq, doublereal *qstore, integer *qptr,
    integer *prmptr, integer *perm, integer *givptr, integer *givcol, doublereal *givnum, doublecomplex *work,
    doublereal *rwork, integer *iwork, integer *info);

int zlaed8_(integer *k, integer *n, integer *qsiz, doublecomplex *q, integer *ldq, doublereal *d__, doublereal *rho,
    integer *cutpnt, doublereal *z__, doublereal *dlamda, doublecomplex *q2, integer *ldq2, doublereal *w,
    integer *indxp, integer *indx, integer *indxq, integer *perm, integer *givptr, integer *givcol, doublereal *givnum,
    integer *info);

int zlaein_(logical *rightv, logical *noinit, integer *n, doublecomplex *h__, integer *ldh, doublecomplex *w,
    doublecomplex *v, doublecomplex *b, integer *ldb, doublereal *rwork, doublereal *eps3, doublereal *smlnum,
    integer *info);

int zlaesy_(doublecomplex *a, doublecomplex *b, doublecomplex *c__, doublecomplex *rt1, doublecomplex *rt2,
    doublecomplex *evscal, doublecomplex *cs1, doublecomplex *sn1);

int zlaev2_(doublecomplex *a, doublecomplex *b, doublecomplex *c__, doublereal *rt1, doublereal *rt2, doublereal *cs1,
    doublecomplex *sn1);

int zlag2c_(integer *m, integer *n, doublecomplex *a, integer *lda, complex *sa, integer *ldsa, integer *info);

int zlags2_(logical *upper, doublereal *a1, doublecomplex *a2, doublereal *a3, doublereal *b1, doublecomplex *b2,
    doublereal *b3, doublereal *csu, doublecomplex *snu, doublereal *csv, doublecomplex *snv, doublereal *csq,
    doublecomplex *snq);

int zlagtm_(char *trans, integer *n, integer *nrhs, doublereal *alpha, doublecomplex *dl, doublecomplex *d__,
    doublecomplex *du, doublecomplex *x, integer *ldx, doublereal *beta, doublecomplex *b, integer *ldb);

int zlahef_(char *uplo, integer *n, integer *nb, integer *kb, doublecomplex *a, integer *lda, integer *ipiv,
    doublecomplex *w, integer *ldw, integer *info);

int zlahqr_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, doublecomplex *h__, integer *ldh,
    doublecomplex *w, integer *iloz, integer *ihiz, doublecomplex *z__, integer *ldz, integer *info);

int zlahr2_(integer *n, integer *k, integer *nb, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *t,
    integer *ldt, doublecomplex *y, integer *ldy);

int zlahrd_(integer *n, integer *k, integer *nb, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *t,
    integer *ldt, doublecomplex *y, integer *ldy);

int zlaic1_(integer *job, integer *j, doublecomplex *x, doublereal *sest, doublecomplex *w, doublecomplex *gamma,
    doublereal *sestpr, doublecomplex *s, doublecomplex *c__);

int zlals0_(integer *icompq, integer *nl, integer *nr, integer *sqre, integer *nrhs, doublecomplex *b, integer *ldb,
    doublecomplex *bx, integer *ldbx, integer *perm, integer *givptr, integer *givcol, integer *ldgcol,
    doublereal *givnum, integer *ldgnum, doublereal *poles, doublereal *difl, doublereal *difr, doublereal *z__,
    integer *k, doublereal *c__, doublereal *s, doublereal *rwork, integer *info);

int zlalsa_(integer *icompq, integer *smlsiz, integer *n, integer *nrhs, doublecomplex *b, integer *ldb,
    doublecomplex *bx, integer *ldbx, doublereal *u, integer *ldu, doublereal *vt, integer *k, doublereal *difl,
    doublereal *difr, doublereal *z__, doublereal *poles, integer *givptr, integer *givcol, integer *ldgcol,
    integer *perm, doublereal *givnum, doublereal *c__, doublereal *s, doublereal *rwork, integer *iwork,
    integer *info);

int zlalsd_(char *uplo, integer *smlsiz, integer *n, integer *nrhs, doublereal *d__, doublereal *e, doublecomplex *b,
    integer *ldb, doublereal *rcond, integer *rank, doublecomplex *work, doublereal *rwork, integer *iwork,
    integer *info);

doublereal zlangb_(
    char *norm, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab, doublereal *work);

doublereal zlange_(char *norm, integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *work);

doublereal zlangt_(char *norm, integer *n, doublecomplex *dl, doublecomplex *d__, doublecomplex *du);

doublereal zlanhb_(char *norm, char *uplo, integer *n, integer *k, doublecomplex *ab, integer *ldab, doublereal *work);

doublereal zlanhe_(char *norm, char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *work);

doublereal zlanhf_(char *norm, char *transr, char *uplo, integer *n, doublecomplex *a, doublereal *work);

doublereal zlanhp_(char *norm, char *uplo, integer *n, doublecomplex *ap, doublereal *work);

doublereal zlanhs_(char *norm, integer *n, doublecomplex *a, integer *lda, doublereal *work);

doublereal zlanht_(char *norm, integer *n, doublereal *d__, doublecomplex *e);

doublereal zlansb_(char *norm, char *uplo, integer *n, integer *k, doublecomplex *ab, integer *ldab, doublereal *work);

doublereal zlansp_(char *norm, char *uplo, integer *n, doublecomplex *ap, doublereal *work);

doublereal zlansy_(char *norm, char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *work);

doublereal zlantb_(
    char *norm, char *uplo, char *diag, integer *n, integer *k, doublecomplex *ab, integer *ldab, doublereal *work);

doublereal zlantp_(char *norm, char *uplo, char *diag, integer *n, doublecomplex *ap, doublereal *work);

doublereal zlantr_(
    char *norm, char *uplo, char *diag, integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *work);

int zlapll_(integer *n, doublecomplex *x, integer *incx, doublecomplex *y, integer *incy, doublereal *ssmin);

int zlapmt_(logical *forwrd, integer *m, integer *n, doublecomplex *x, integer *ldx, integer *k);

int zlaqgb_(integer *m, integer *n, integer *kl, integer *ku, doublecomplex *ab, integer *ldab, doublereal *r__,
    doublereal *c__, doublereal *rowcnd, doublereal *colcnd, doublereal *amax, char *equed);

int zlaqge_(integer *m, integer *n, doublecomplex *a, integer *lda, doublereal *r__, doublereal *c__,
    doublereal *rowcnd, doublereal *colcnd, doublereal *amax, char *equed);

int zlaqhb_(char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, doublereal *s, doublereal *scond,
    doublereal *amax, char *equed);

int zlaqhe_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax,
    char *equed);

int zlaqhp_(char *uplo, integer *n, doublecomplex *ap, doublereal *s, doublereal *scond, doublereal *amax, char *equed);

int zlaqp2_(integer *m, integer *n, integer *offset, doublecomplex *a, integer *lda, integer *jpvt, doublecomplex *tau,
    doublereal *vn1, doublereal *vn2, doublecomplex *work);

int zlaqps_(integer *m, integer *n, integer *offset, integer *nb, integer *kb, doublecomplex *a, integer *lda,
    integer *jpvt, doublecomplex *tau, doublereal *vn1, doublereal *vn2, doublecomplex *auxv, doublecomplex *f,
    integer *ldf);

int zlaqr0_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, doublecomplex *h__, integer *ldh,
    doublecomplex *w, integer *iloz, integer *ihiz, doublecomplex *z__, integer *ldz, doublecomplex *work,
    integer *lwork, integer *info);

int zlaqr1_(integer *n, doublecomplex *h__, integer *ldh, doublecomplex *s1, doublecomplex *s2, doublecomplex *v);

int zlaqr2_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, doublecomplex *h__,
    integer *ldh, integer *iloz, integer *ihiz, doublecomplex *z__, integer *ldz, integer *ns, integer *nd,
    doublecomplex *sh, doublecomplex *v, integer *ldv, integer *nh, doublecomplex *t, integer *ldt, integer *nv,
    doublecomplex *wv, integer *ldwv, doublecomplex *work, integer *lwork);

int zlaqr3_(logical *wantt, logical *wantz, integer *n, integer *ktop, integer *kbot, integer *nw, doublecomplex *h__,
    integer *ldh, integer *iloz, integer *ihiz, doublecomplex *z__, integer *ldz, integer *ns, integer *nd,
    doublecomplex *sh, doublecomplex *v, integer *ldv, integer *nh, doublecomplex *t, integer *ldt, integer *nv,
    doublecomplex *wv, integer *ldwv, doublecomplex *work, integer *lwork);

int zlaqr4_(logical *wantt, logical *wantz, integer *n, integer *ilo, integer *ihi, doublecomplex *h__, integer *ldh,
    doublecomplex *w, integer *iloz, integer *ihiz, doublecomplex *z__, integer *ldz, doublecomplex *work,
    integer *lwork, integer *info);

int zlaqr5_(logical *wantt, logical *wantz, integer *kacc22, integer *n, integer *ktop, integer *kbot, integer *nshfts,
    doublecomplex *s, doublecomplex *h__, integer *ldh, integer *iloz, integer *ihiz, doublecomplex *z__, integer *ldz,
    doublecomplex *v, integer *ldv, doublecomplex *u, integer *ldu, integer *nv, doublecomplex *wv, integer *ldwv,
    integer *nh, doublecomplex *wh, integer *ldwh);

int zlaqsb_(char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, doublereal *s, doublereal *scond,
    doublereal *amax, char *equed);

int zlaqsp_(char *uplo, integer *n, doublecomplex *ap, doublereal *s, doublereal *scond, doublereal *amax, char *equed);

int zlaqsy_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax,
    char *equed);

int zlar1v_(integer *n, integer *b1, integer *bn, doublereal *lambda, doublereal *d__, doublereal *l, doublereal *ld,
    doublereal *lld, doublereal *pivmin, doublereal *gaptol, doublecomplex *z__, logical *wantnc, integer *negcnt,
    doublereal *ztz, doublereal *mingma, integer *r__, integer *isuppz, doublereal *nrminv, doublereal *resid,
    doublereal *rqcorr, doublereal *work);

int zlar2v_(integer *n, doublecomplex *x, doublecomplex *y, doublecomplex *z__, integer *incx, doublereal *c__,
    doublecomplex *s, integer *incc);

int zlarcm_(integer *m, integer *n, doublereal *a, integer *lda, doublecomplex *b, integer *ldb, doublecomplex *c__,
    integer *ldc, doublereal *rwork);

int zlarf_(char *side, integer *m, integer *n, doublecomplex *v, integer *incv, doublecomplex *tau, doublecomplex *c__,
    integer *ldc, doublecomplex *work);

int zlarfb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, doublecomplex *v,
    integer *ldv, doublecomplex *t, integer *ldt, doublecomplex *c__, integer *ldc, doublecomplex *work,
    integer *ldwork);

int zlarfg_(integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *tau);

int zlarfp_(integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *tau);

int zlarft_(char *direct, char *storev, integer *n, integer *k, doublecomplex *v, integer *ldv, doublecomplex *tau,
    doublecomplex *t, integer *ldt);

int zlarfx_(char *side, integer *m, integer *n, doublecomplex *v, doublecomplex *tau, doublecomplex *c__, integer *ldc,
    doublecomplex *work);

int zlargv_(
    integer *n, doublecomplex *x, integer *incx, doublecomplex *y, integer *incy, doublereal *c__, integer *incc);

int zlarnv_(integer *idist, integer *iseed, integer *n, doublecomplex *x);

int zlarrv_(integer *n, doublereal *vl, doublereal *vu, doublereal *d__, doublereal *l, doublereal *pivmin,
    integer *isplit, integer *m, integer *dol, integer *dou, doublereal *minrgp, doublereal *rtol1, doublereal *rtol2,
    doublereal *w, doublereal *werr, doublereal *wgap, integer *iblock, integer *indexw, doublereal *gers,
    doublecomplex *z__, integer *ldz, integer *isuppz, doublereal *work, integer *iwork, integer *info);

int zlarscl2_(integer *m, integer *n, doublereal *d__, doublecomplex *x, integer *ldx);

int zlartg_(doublecomplex *f, doublecomplex *g, doublereal *cs, doublecomplex *sn, doublecomplex *r__);

int zlartv_(integer *n, doublecomplex *x, integer *incx, doublecomplex *y, integer *incy, doublereal *c__,
    doublecomplex *s, integer *incc);

int zlarz_(char *side, integer *m, integer *n, integer *l, doublecomplex *v, integer *incv, doublecomplex *tau,
    doublecomplex *c__, integer *ldc, doublecomplex *work);

int zlarzb_(char *side, char *trans, char *direct, char *storev, integer *m, integer *n, integer *k, integer *l,
    doublecomplex *v, integer *ldv, doublecomplex *t, integer *ldt, doublecomplex *c__, integer *ldc,
    doublecomplex *work, integer *ldwork);

int zlarzt_(char *direct, char *storev, integer *n, integer *k, doublecomplex *v, integer *ldv, doublecomplex *tau,
    doublecomplex *t, integer *ldt);

int zlascl_(char *type__, integer *kl, integer *ku, doublereal *cfrom, doublereal *cto, integer *m, integer *n,
    doublecomplex *a, integer *lda, integer *info);

int zlascl2_(integer *m, integer *n, doublereal *d__, doublecomplex *x, integer *ldx);

int zlaset_(
    char *uplo, integer *m, integer *n, doublecomplex *alpha, doublecomplex *beta, doublecomplex *a, integer *lda);

int zlasr_(char *side, char *pivot, char *direct, integer *m, integer *n, doublereal *c__, doublereal *s,
    doublecomplex *a, integer *lda);

int zlassq_(integer *n, doublecomplex *x, integer *incx, doublereal *scale, doublereal *sumsq);

int zlaswp_(integer *n, doublecomplex *a, integer *lda, integer *k1, integer *k2, integer *ipiv, integer *incx);

int zlasyf_(char *uplo, integer *n, integer *nb, integer *kb, doublecomplex *a, integer *lda, integer *ipiv,
    doublecomplex *w, integer *ldw, integer *info);

int zlat2c_(char *uplo, integer *n, doublecomplex *a, integer *lda, complex *sa, integer *ldsa, integer *info);

int zlatbs_(char *uplo, char *trans, char *diag, char *normin, integer *n, integer *kd, doublecomplex *ab,
    integer *ldab, doublecomplex *x, doublereal *scale, doublereal *cnorm, integer *info);

int zlatdf_(integer *ijob, integer *n, doublecomplex *z__, integer *ldz, doublecomplex *rhs, doublereal *rdsum,
    doublereal *rdscal, integer *ipiv, integer *jpiv);

int zlatps_(char *uplo, char *trans, char *diag, char *normin, integer *n, doublecomplex *ap, doublecomplex *x,
    doublereal *scale, doublereal *cnorm, integer *info);

int zlatrd_(char *uplo, integer *n, integer *nb, doublecomplex *a, integer *lda, doublereal *e, doublecomplex *tau,
    doublecomplex *w, integer *ldw);

int zlatrs_(char *uplo, char *trans, char *diag, char *normin, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *x, doublereal *scale, doublereal *cnorm, integer *info);

int zlatrz_(
    integer *m, integer *n, integer *l, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work);

int zlatzm_(char *side, integer *m, integer *n, doublecomplex *v, integer *incv, doublecomplex *tau, doublecomplex *c1,
    doublecomplex *c2, integer *ldc, doublecomplex *work);

int zlauu2_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *info);

int zlauum_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *info);

int zpbcon_(char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, doublereal *anorm, doublereal *rcond,
    doublecomplex *work, doublereal *rwork, integer *info);

int zpbequ_(char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, doublereal *s, doublereal *scond,
    doublereal *amax, integer *info);

int zpbrfs_(char *uplo, integer *n, integer *kd, integer *nrhs, doublecomplex *ab, integer *ldab, doublecomplex *afb,
    integer *ldafb, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zpbstf_(char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, integer *info);

int zpbsv_(char *uplo, integer *n, integer *kd, integer *nrhs, doublecomplex *ab, integer *ldab, doublecomplex *b,
    integer *ldb, integer *info);

int zpbsvx_(char *fact, char *uplo, integer *n, integer *kd, integer *nrhs, doublecomplex *ab, integer *ldab,
    doublecomplex *afb, integer *ldafb, char *equed, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x,
    integer *ldx, doublereal *rcond, doublereal *ferr, doublereal *berr, doublecomplex *work, doublereal *rwork,
    integer *info);

int zpbtf2_(char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, integer *info);

int zpbtrf_(char *uplo, integer *n, integer *kd, doublecomplex *ab, integer *ldab, integer *info);

int zpbtrs_(char *uplo, integer *n, integer *kd, integer *nrhs, doublecomplex *ab, integer *ldab, doublecomplex *b,
    integer *ldb, integer *info);

int zpftrf_(char *transr, char *uplo, integer *n, doublecomplex *a, integer *info);

int zpftri_(char *transr, char *uplo, integer *n, doublecomplex *a, integer *info);

int zpftrs_(char *transr, char *uplo, integer *n, integer *nrhs, doublecomplex *a, doublecomplex *b, integer *ldb,
    integer *info);

int zpocon_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *anorm, doublereal *rcond,
    doublecomplex *work, doublereal *rwork, integer *info);

int zpoequ_(
    integer *n, doublecomplex *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax, integer *info);

int zpoequb_(
    integer *n, doublecomplex *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax, integer *info);

int zporfs_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zporfsx_(char *uplo, char *equed, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond,
    doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams,
    doublereal *params, doublecomplex *work, doublereal *rwork, integer *info);

int zposv_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    integer *info);

int zposvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, char *equed, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx,
    doublereal *rcond, doublereal *ferr, doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zposvxx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, char *equed, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx,
    doublereal *rcond, doublereal *rpvgrw, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublecomplex *work, doublereal *rwork,
    integer *info);

int zpotf2_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *info);

int zpotrf_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *info);

int zpotri_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *info);

int zpotrs_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    integer *info);

int zppcon_(char *uplo, integer *n, doublecomplex *ap, doublereal *anorm, doublereal *rcond, doublecomplex *work,
    doublereal *rwork, integer *info);

int zppequ_(
    char *uplo, integer *n, doublecomplex *ap, doublereal *s, doublereal *scond, doublereal *amax, integer *info);

int zpprfs_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *afp, doublecomplex *b,
    integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr, doublecomplex *work,
    doublereal *rwork, integer *info);

int zppsv_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *b, integer *ldb, integer *info);

int zppsvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *afp, char *equed,
    doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *ferr,
    doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zpptrf_(char *uplo, integer *n, doublecomplex *ap, integer *info);

int zpptri_(char *uplo, integer *n, doublecomplex *ap, integer *info);

int zpptrs_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *b, integer *ldb, integer *info);

int zpstf2_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *piv, integer *rank, doublereal *tol,
    doublereal *work, integer *info);

int zpstrf_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *piv, integer *rank, doublereal *tol,
    doublereal *work, integer *info);

int zptcon_(integer *n, doublereal *d__, doublecomplex *e, doublereal *anorm, doublereal *rcond, doublereal *rwork,
    integer *info);

int zpteqr_(char *compz, integer *n, doublereal *d__, doublereal *e, doublecomplex *z__, integer *ldz, doublereal *work,
    integer *info);

int zptrfs_(char *uplo, integer *n, integer *nrhs, doublereal *d__, doublecomplex *e, doublereal *df, doublecomplex *ef,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zptsv_(integer *n, integer *nrhs, doublereal *d__, doublecomplex *e, doublecomplex *b, integer *ldb, integer *info);

int zptsvx_(char *fact, integer *n, integer *nrhs, doublereal *d__, doublecomplex *e, doublereal *df, doublecomplex *ef,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *ferr,
    doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zpttrf_(integer *n, doublereal *d__, doublecomplex *e, integer *info);

int zpttrs_(char *uplo, integer *n, integer *nrhs, doublereal *d__, doublecomplex *e, doublecomplex *b, integer *ldb,
    integer *info);

int zptts2_(
    integer *iuplo, integer *n, integer *nrhs, doublereal *d__, doublecomplex *e, doublecomplex *b, integer *ldb);

int zrot_(
    integer *n, doublecomplex *cx, integer *incx, doublecomplex *cy, integer *incy, doublereal *c__, doublecomplex *s);

int zspcon_(char *uplo, integer *n, doublecomplex *ap, integer *ipiv, doublereal *anorm, doublereal *rcond,
    doublecomplex *work, integer *info);

int zspmv_(char *uplo, integer *n, doublecomplex *alpha, doublecomplex *ap, doublecomplex *x, integer *incx,
    doublecomplex *beta, doublecomplex *y, integer *incy);

int zspr_(char *uplo, integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *ap);

int zsprfs_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *afp, integer *ipiv,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zspsv_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, integer *ipiv, doublecomplex *b, integer *ldb,
    integer *info);

int zspsvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *afp, integer *ipiv,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond, doublereal *ferr,
    doublereal *berr, doublecomplex *work, doublereal *rwork, integer *info);

int zsptrf_(char *uplo, integer *n, doublecomplex *ap, integer *ipiv, integer *info);

int zsptri_(char *uplo, integer *n, doublecomplex *ap, integer *ipiv, doublecomplex *work, integer *info);

int zsptrs_(char *uplo, integer *n, integer *nrhs, doublecomplex *ap, integer *ipiv, doublecomplex *b, integer *ldb,
    integer *info);

int zstedc_(char *compz, integer *n, doublereal *d__, doublereal *e, doublecomplex *z__, integer *ldz,
    doublecomplex *work, integer *lwork, doublereal *rwork, integer *lrwork, integer *iwork, integer *liwork,
    integer *info);

int zstegr_(char *jobz, char *range, integer *n, doublereal *d__, doublereal *e, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, doublereal *abstol, integer *m, doublereal *w, doublecomplex *z__, integer *ldz,
    integer *isuppz, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int zstein_(integer *n, doublereal *d__, doublereal *e, integer *m, doublereal *w, integer *iblock, integer *isplit,
    doublecomplex *z__, integer *ldz, doublereal *work, integer *iwork, integer *ifail, integer *info);

int zstemr_(char *jobz, char *range, integer *n, doublereal *d__, doublereal *e, doublereal *vl, doublereal *vu,
    integer *il, integer *iu, integer *m, doublereal *w, doublecomplex *z__, integer *ldz, integer *nzc,
    integer *isuppz, logical *tryrac, doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int zsteqr_(char *compz, integer *n, doublereal *d__, doublereal *e, doublecomplex *z__, integer *ldz, doublereal *work,
    integer *info);

int zsycon_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, doublereal *anorm, doublereal *rcond,
    doublecomplex *work, integer *info);

int zsyequb_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublereal *s, doublereal *scond, doublereal *amax,
    doublecomplex *work, integer *info);

int zsymv_(char *uplo, integer *n, doublecomplex *alpha, doublecomplex *a, integer *lda, doublecomplex *x,
    integer *incx, doublecomplex *beta, doublecomplex *y, integer *incy);

int zsyr_(
    char *uplo, integer *n, doublecomplex *alpha, doublecomplex *x, integer *incx, doublecomplex *a, integer *lda);

int zsyrfs_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af, integer *ldaf,
    integer *ipiv, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int zsyrfsx_(char *uplo, char *equed, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx,
    doublereal *rcond, doublereal *berr, integer *n_err_bnds__, doublereal *err_bnds_norm__,
    doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublecomplex *work, doublereal *rwork,
    integer *info);

int zsysv_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *b,
    integer *ldb, doublecomplex *work, integer *lwork, integer *info);

int zsysvx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *rcond,
    doublereal *ferr, doublereal *berr, doublecomplex *work, integer *lwork, doublereal *rwork, integer *info);

int zsysvxx_(char *fact, char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *af,
    integer *ldaf, integer *ipiv, char *equed, doublereal *s, doublecomplex *b, integer *ldb, doublecomplex *x,
    integer *ldx, doublereal *rcond, doublereal *rpvgrw, doublereal *berr, integer *n_err_bnds__,
    doublereal *err_bnds_norm__, doublereal *err_bnds_comp__, integer *nparams, doublereal *params, doublecomplex *work,
    doublereal *rwork, integer *info);

int zsytf2_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, integer *info);

int zsytrf_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *work, integer *lwork,
    integer *info);

int zsytri_(char *uplo, integer *n, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *work, integer *info);

int zsytrs_(char *uplo, integer *n, integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, doublecomplex *b,
    integer *ldb, integer *info);

int ztbcon_(char *norm, char *uplo, char *diag, integer *n, integer *kd, doublecomplex *ab, integer *ldab,
    doublereal *rcond, doublecomplex *work, doublereal *rwork, integer *info);

int ztbrfs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, doublecomplex *ab,
    integer *ldab, doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int ztbtrs_(char *uplo, char *trans, char *diag, integer *n, integer *kd, integer *nrhs, doublecomplex *ab,
    integer *ldab, doublecomplex *b, integer *ldb, integer *info);

int ztfsm_(char *transr, char *side, char *uplo, char *trans, char *diag, integer *m, integer *n, doublecomplex *alpha,
    doublecomplex *a, doublecomplex *b, integer *ldb);

int ztftri_(char *transr, char *uplo, char *diag, integer *n, doublecomplex *a, integer *info);

int ztfttp_(char *transr, char *uplo, integer *n, doublecomplex *arf, doublecomplex *ap, integer *info);

int ztfttr_(char *transr, char *uplo, integer *n, doublecomplex *arf, doublecomplex *a, integer *lda, integer *info);

int ztgevc_(char *side, char *howmny, logical *select, integer *n, doublecomplex *s, integer *lds, doublecomplex *p,
    integer *ldp, doublecomplex *vl, integer *ldvl, doublecomplex *vr, integer *ldvr, integer *mm, integer *m,
    doublecomplex *work, doublereal *rwork, integer *info);

int ztgex2_(logical *wantq, logical *wantz, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *q, integer *ldq, doublecomplex *z__, integer *ldz, integer *j1, integer *info);

int ztgexc_(logical *wantq, logical *wantz, integer *n, doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb,
    doublecomplex *q, integer *ldq, doublecomplex *z__, integer *ldz, integer *ifst, integer *ilst, integer *info);

int ztgsen_(integer *ijob, logical *wantq, logical *wantz, logical *select, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *alpha, doublecomplex *beta, doublecomplex *q, integer *ldq,
    doublecomplex *z__, integer *ldz, integer *m, doublereal *pl, doublereal *pr, doublereal *dif, doublecomplex *work,
    integer *lwork, integer *iwork, integer *liwork, integer *info);

int ztgsja_(char *jobu, char *jobv, char *jobq, integer *m, integer *p, integer *n, integer *k, integer *l,
    doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb, doublereal *tola, doublereal *tolb,
    doublereal *alpha, doublereal *beta, doublecomplex *u, integer *ldu, doublecomplex *v, integer *ldv,
    doublecomplex *q, integer *ldq, doublecomplex *work, integer *ncycle, integer *info);

int ztgsna_(char *job, char *howmny, logical *select, integer *n, doublecomplex *a, integer *lda, doublecomplex *b,
    integer *ldb, doublecomplex *vl, integer *ldvl, doublecomplex *vr, integer *ldvr, doublereal *s, doublereal *dif,
    integer *mm, integer *m, doublecomplex *work, integer *lwork, integer *iwork, integer *info);

int ztgsy2_(char *trans, integer *ijob, integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *b,
    integer *ldb, doublecomplex *c__, integer *ldc, doublecomplex *d__, integer *ldd, doublecomplex *e, integer *lde,
    doublecomplex *f, integer *ldf, doublereal *scale, doublereal *rdsum, doublereal *rdscal, integer *info);

int ztgsyl_(char *trans, integer *ijob, integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *b,
    integer *ldb, doublecomplex *c__, integer *ldc, doublecomplex *d__, integer *ldd, doublecomplex *e, integer *lde,
    doublecomplex *f, integer *ldf, doublereal *scale, doublereal *dif, doublecomplex *work, integer *lwork,
    integer *iwork, integer *info);

int ztpcon_(char *norm, char *uplo, char *diag, integer *n, doublecomplex *ap, doublereal *rcond, doublecomplex *work,
    doublereal *rwork, integer *info);

int ztprfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *b,
    integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr, doublecomplex *work,
    doublereal *rwork, integer *info);

int ztptri_(char *uplo, char *diag, integer *n, doublecomplex *ap, integer *info);

int ztptrs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublecomplex *ap, doublecomplex *b,
    integer *ldb, integer *info);

int ztpttf_(char *transr, char *uplo, integer *n, doublecomplex *ap, doublecomplex *arf, integer *info);

int ztpttr_(char *uplo, integer *n, doublecomplex *ap, doublecomplex *a, integer *lda, integer *info);

int ztrcon_(char *norm, char *uplo, char *diag, integer *n, doublecomplex *a, integer *lda, doublereal *rcond,
    doublecomplex *work, doublereal *rwork, integer *info);

int ztrevc_(char *side, char *howmny, logical *select, integer *n, doublecomplex *t, integer *ldt, doublecomplex *vl,
    integer *ldvl, doublecomplex *vr, integer *ldvr, integer *mm, integer *m, doublecomplex *work, doublereal *rwork,
    integer *info);

int ztrexc_(char *compq, integer *n, doublecomplex *t, integer *ldt, doublecomplex *q, integer *ldq, integer *ifst,
    integer *ilst, integer *info);

int ztrrfs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, doublereal *ferr, doublereal *berr,
    doublecomplex *work, doublereal *rwork, integer *info);

int ztrsen_(char *job, char *compq, logical *select, integer *n, doublecomplex *t, integer *ldt, doublecomplex *q,
    integer *ldq, doublecomplex *w, integer *m, doublereal *s, doublereal *sep, doublecomplex *work, integer *lwork,
    integer *info);

int ztrsna_(char *job, char *howmny, logical *select, integer *n, doublecomplex *t, integer *ldt, doublecomplex *vl,
    integer *ldvl, doublecomplex *vr, integer *ldvr, doublereal *s, doublereal *sep, integer *mm, integer *m,
    doublecomplex *work, integer *ldwork, doublereal *rwork, integer *info);

int ztrsyl_(char *trana, char *tranb, integer *isgn, integer *m, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, doublecomplex *c__, integer *ldc, doublereal *scale, integer *info);

int ztrti2_(char *uplo, char *diag, integer *n, doublecomplex *a, integer *lda, integer *info);

int ztrtri_(char *uplo, char *diag, integer *n, doublecomplex *a, integer *lda, integer *info);

int ztrtrs_(char *uplo, char *trans, char *diag, integer *n, integer *nrhs, doublecomplex *a, integer *lda,
    doublecomplex *b, integer *ldb, integer *info);

int ztrttf_(char *transr, char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *arf, integer *info);

int ztrttp_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *ap, integer *info);

int ztzrqf_(integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, integer *info);

int ztzrzf_(integer *m, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zung2l_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *info);

int zung2r_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *info);

int zungbr_(char *vect, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau,
    doublecomplex *work, integer *lwork, integer *info);

int zunghr_(integer *n, integer *ilo, integer *ihi, doublecomplex *a, integer *lda, doublecomplex *tau,
    doublecomplex *work, integer *lwork, integer *info);

int zungl2_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *info);

int zunglq_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zungql_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zungqr_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zungr2_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *info);

int zungrq_(integer *m, integer *n, integer *k, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zungtr_(char *uplo, integer *n, doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *work,
    integer *lwork, integer *info);

int zunm2l_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *info);

int zunm2r_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *info);

int zunmbr_(char *vect, char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zunmhr_(char *side, char *trans, integer *m, integer *n, integer *ilo, integer *ihi, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zunml2_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *info);

int zunmlq_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zunmql_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zunmqr_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zunmr2_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *info);

int zunmr3_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *info);

int zunmrq_(char *side, char *trans, integer *m, integer *n, integer *k, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zunmrz_(char *side, char *trans, integer *m, integer *n, integer *k, integer *l, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zunmtr_(char *side, char *uplo, char *trans, integer *m, integer *n, doublecomplex *a, integer *lda,
    doublecomplex *tau, doublecomplex *c__, integer *ldc, doublecomplex *work, integer *lwork, integer *info);

int zupgtr_(char *uplo, integer *n, doublecomplex *ap, doublecomplex *tau, doublecomplex *q, integer *ldq,
    doublecomplex *work, integer *info);

int zupmtr_(char *side, char *uplo, char *trans, integer *m, integer *n, doublecomplex *ap, doublecomplex *tau,
    doublecomplex *c__, integer *ldc, doublecomplex *work, integer *info);

int dlamc1_(integer *beta, integer *t, logical *rnd, logical *ieee1);

doublereal dsecnd_(void);

int ilaver_(integer *vers_major__, integer *vers_minor__, integer *vers_patch__);

logical lsame_(char *ca, char *cb);

doublereal second_(void);

doublereal slamch_(char *cmach);

int slamc1_(integer *beta, integer *t, logical *rnd, logical *ieee1);

int slamc2_(integer *beta, integer *t, logical *rnd, real *eps, integer *emin, real *rmin, integer *emax, real *rmax);

doublereal slamc3_(real *a, real *b);

int slamc4_(integer *emin, real *start, integer *base);

int slamc5_(integer *beta, integer *p, integer *emin, logical *ieee, integer *emax, real *rmax);

doublereal dlamch_(char *cmach);

int dlamc1_(integer *beta, integer *t, logical *rnd, logical *ieee1);

int dlamc2_(integer *beta, integer *t, logical *rnd, doublereal *eps, integer *emin, doublereal *rmin, integer *emax,
    doublereal *rmax);

doublereal dlamc3_(doublereal *a, doublereal *b);

int dlamc4_(integer *emin, doublereal *start, integer *base);

int dlamc5_(integer *beta, integer *p, integer *emin, logical *ieee, integer *emax, doublereal *rmax);

integer ilaenv_(integer *ispec, char *name__, char *opts, integer *n1, integer *n2, integer *n3, integer *n4);

#endif /* __CLAPACK_H */
